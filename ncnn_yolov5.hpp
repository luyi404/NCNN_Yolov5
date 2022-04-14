#include "NCNN_Windows.h"


#define MAX_STRIDE 32
#define USE_INT8

namespace yolov5 {
	
	struct Object
	{
		cv::Rect_<float> rect;
		int label;
		float prob;
	};

	static inline float intersection_area(const Object& a, const Object& b)
	{
		cv::Rect_<float> inter = a.rect & b.rect;
		return inter.area();
	}

	static void qsort_descent_inplace(std::vector<Object>& faceobjects, int left, int right)
	{
		int i = left;
		int j = right;
		float p = faceobjects[(left + right) / 2].prob;

		while (i <= j)
		{
			while (faceobjects[i].prob > p)
				i++;

			while (faceobjects[j].prob < p)
				j--;

			if (i <= j)
			{
				// swap
				std::swap(faceobjects[i], faceobjects[j]);

				i++;
				j--;
			}
		}

		#pragma omp parallel sections
		{
			#pragma omp section
			{
				if (left < j) qsort_descent_inplace(faceobjects, left, j);
			}
			#pragma omp section
			{
				if (i < right) qsort_descent_inplace(faceobjects, i, right);
			}
		}
	}

	static void qsort_descent_inplace(std::vector<Object>& faceobjects)
	{
		if (faceobjects.empty())
			return;

		qsort_descent_inplace(faceobjects, 0, faceobjects.size() - 1);
	}

	static void nms_sorted_bboxes(const std::vector<Object>& faceobjects, std::vector<int>& picked, float nms_threshold)
	{
		picked.clear();

		const int n = faceobjects.size();

		std::vector<float> areas(n);
		for (int i = 0; i < n; i++)
		{
			areas[i] = faceobjects[i].rect.area();
		}

		for (int i = 0; i < n; i++)
		{
			const Object& a = faceobjects[i];

			int keep = 1;
			for (int j = 0; j < (int)picked.size(); j++)
			{
				const Object& b = faceobjects[picked[j]];

				// intersection over union
				float inter_area = intersection_area(a, b);
				float union_area = areas[i] + areas[picked[j]] - inter_area;
				// float IoU = inter_area / union_area
				if (inter_area / union_area > nms_threshold)
					keep = 0;
			}

			if (keep)
				picked.push_back(i);
		}
	}

	static inline float sigmoid(float x)
	{
		return static_cast<float>(1.f / (1.f + exp(-x)));
	}
	
	static void generate_proposals(const ncnn::Mat& anchors, int stride, const ncnn::Mat& in_pad, const ncnn::Mat& feat_blob, float prob_threshold, std::vector<Object>& objects)
	{
		const int num_grid = feat_blob.h;

		int num_grid_x;
		int num_grid_y;
		if (in_pad.w > in_pad.h)
		{
			num_grid_x = in_pad.w / stride;
			num_grid_y = num_grid / num_grid_x;
		}
		else
		{
			num_grid_y = in_pad.h / stride;
			num_grid_x = num_grid / num_grid_y;
		}

		const int num_class = feat_blob.w - 5;

		const int num_anchors = anchors.w / 2;

		for (int q = 0; q < num_anchors; q++)
		{
			const float anchor_w = anchors[q * 2];
			const float anchor_h = anchors[q * 2 + 1];

			const ncnn::Mat feat = feat_blob.channel(q);

			for (int i = 0; i < num_grid_y; i++)
			{
				for (int j = 0; j < num_grid_x; j++)
				{
					const float* featptr = feat.row(i * num_grid_x + j);

					// find class index with max class score
					int class_index = 0;
					float class_score = -FLT_MAX;
					for (int k = 0; k < num_class; k++)
					{
						float score = featptr[5 + k];
						if (score > class_score)
						{
							class_index = k;
							class_score = score;
						}
					}

					float box_score = featptr[4];

					float confidence = sigmoid(box_score) * sigmoid(class_score);

					if (confidence >= prob_threshold)
					{
						// yolov5/models/yolo.py Detect forward
						// y = x[i].sigmoid()
						// y[..., 0:2] = (y[..., 0:2] * 2. - 0.5 + self.grid[i].to(x[i].device)) * self.stride[i]  # xy
						// y[..., 2:4] = (y[..., 2:4] * 2) ** 2 * self.anchor_grid[i]  # wh

						float dx = sigmoid(featptr[0]);
						float dy = sigmoid(featptr[1]);
						float dw = sigmoid(featptr[2]);
						float dh = sigmoid(featptr[3]);

						float pb_cx = (dx * 2.f - 0.5f + j) * stride;
						float pb_cy = (dy * 2.f - 0.5f + i) * stride;

						float pb_w = pow(dw * 2.f, 2) * anchor_w;
						float pb_h = pow(dh * 2.f, 2) * anchor_h;

						float x0 = pb_cx - pb_w * 0.5f;
						float y0 = pb_cy - pb_h * 0.5f;
						float x1 = pb_cx + pb_w * 0.5f;
						float y1 = pb_cy + pb_h * 0.5f;

						Object obj;
						obj.rect.x = x0;
						obj.rect.y = y0;
						obj.rect.width = x1 - x0;
						obj.rect.height = y1 - y0;
						obj.label = class_index;
						obj.prob = confidence;

						objects.push_back(obj);
					}
				}
			}
		}
	}

	static int detect_yolov5(const char* param_path, const char* bin_path, const cv::Mat& bgr, std::vector<Object>& objects)
	{
		ncnn::Net yolov5;

		// yolov5.opt.use_vulkan_compute = true; //这一行控制是否使用gpu


		// original pretrained model from https://github.com/ultralytics/yolov5
		// the ncnn model https://github.com/nihui/ncnn-assets/tree/master/models

		yolov5.load_param(param_path);
		yolov5.load_model(bin_path);

#ifdef USE_INT8
		yolov5.opt.num_threads = 8;
		yolov5.opt.use_int8_inference = true;
#endif

		const int target_size = 320;
		const float prob_threshold = 0.4f;
		const float nms_threshold = 0.5f;

		int img_w = bgr.cols;
		int img_h = bgr.rows;

		// letterbox pad to multiple of MAX_STRIDE
		int w = img_w;
		int h = img_h;
		float scale = 1.f;
		/*
		if (w > h)
		{
			scale = (float)target_size / w;
			w = target_size;
			h = h * scale;
		}
		else
		{
			scale = (float)target_size / h;
			h = target_size;
			w = w * scale;
		}
		*/

		float scale_w = (float)target_size / w;
		float scale_h = (float)target_size / h;
		w = h = target_size;

		ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR2RGB, img_w, img_h, w, h);

		// pad to target_size rectangle
		// yolov5/utils/datasets.py letterbox
		int wpad = (w + MAX_STRIDE - 1) / MAX_STRIDE * MAX_STRIDE - w;
		int hpad = (h + MAX_STRIDE - 1) / MAX_STRIDE * MAX_STRIDE - h;
		ncnn::Mat in_pad;
		ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 114.f);

		const float norm_vals[3] = { 1 / 255.f, 1 / 255.f, 1 / 255.f };
		in_pad.substract_mean_normalize(0, norm_vals);
		
		std::cout << "After preprocess: " << in_pad.c << " " << in_pad.w << " " << in_pad.h << std::endl;
		

		ncnn::Extractor ex = yolov5.create_extractor();

		ex.input("images", in_pad);

		std::vector<Object> proposals;

		// anchor setting from yolov5/models/yolov5s.yaml

		// stride 8
		{
			ncnn::Mat out;
			ex.extract("2214", out);

			ncnn::Mat anchors(6);
			anchors[0] = 10.f;
			anchors[1] = 13.f;
			anchors[2] = 16.f;
			anchors[3] = 30.f;
			anchors[4] = 33.f;
			anchors[5] = 23.f;

			std::vector<Object> objects8;
			generate_proposals(anchors, 8, in_pad, out, prob_threshold, objects8);

			proposals.insert(proposals.end(), objects8.begin(), objects8.end());
		}

		// stride 16
		{
			ncnn::Mat out;
			ex.extract("2240", out);

			ncnn::Mat anchors(6);
			anchors[0] = 30.f;
			anchors[1] = 61.f;
			anchors[2] = 62.f;
			anchors[3] = 45.f;
			anchors[4] = 59.f;
			anchors[5] = 119.f;

			std::vector<Object> objects16;
			generate_proposals(anchors, 16, in_pad, out, prob_threshold, objects16);

			proposals.insert(proposals.end(), objects16.begin(), objects16.end());
		}

		// stride 32
		{
			ncnn::Mat out;
			ex.extract("2266", out);
			
			ncnn::Mat anchors(6);
			anchors[0] = 116.f;
			anchors[1] = 90.f;
			anchors[2] = 156.f;
			anchors[3] = 198.f;
			anchors[4] = 373.f;
			anchors[5] = 326.f;

			std::vector<Object> objects32;
			generate_proposals(anchors, 32, in_pad, out, prob_threshold, objects32);

			proposals.insert(proposals.end(), objects32.begin(), objects32.end());
		}

		// sort all proposals by score from highest to lowest
		qsort_descent_inplace(proposals);

		// apply nms with nms_threshold
		std::vector<int> picked;
		nms_sorted_bboxes(proposals, picked, nms_threshold);

		size_t count = picked.size();

		objects.resize(count);
		for (int i = 0; i < count; i++)
		{
			objects[i] = proposals[picked[i]];

			// adjust offset to original unpadded
			float x0 = (objects[i].rect.x - (wpad / 2)) / scale_w;
			float y0 = (objects[i].rect.y - (hpad / 2)) / scale_h;
			float x1 = (objects[i].rect.x + objects[i].rect.width - (wpad / 2)) / scale_w;
			float y1 = (objects[i].rect.y + objects[i].rect.height - (hpad / 2)) / scale_h;

			// clip
			x0 = std::max(std::min(x0, (float)(img_w - 1)), 0.f);
			y0 = std::max(std::min(y0, (float)(img_h - 1)), 0.f);
			x1 = std::max(std::min(x1, (float)(img_w - 1)), 0.f);
			y1 = std::max(std::min(y1, (float)(img_h - 1)), 0.f);

			objects[i].rect.x = x0;
			objects[i].rect.y = y0;
			objects[i].rect.width = x1 - x0;
			objects[i].rect.height = y1 - y0;
		}

		return 0;
	}

	static void draw_objects(const cv::Mat& bgr, const std::vector<Object>& objects)
	{
		static const char* class_names[] = {
			"bj_bpmh","bj_bpps","bj_wkps","hxq_gjtps","hxq_yfyc"
		};

		cv::Mat image = bgr.clone();

		for (size_t i = 0; i < objects.size(); i++)
		{
			const Object& obj = objects[i];

			fprintf(stderr, "%d = %.5f at %.2f %.2f %.2f x %.2f\n", obj.label, obj.prob,
				obj.rect.x, obj.rect.y, obj.rect.width, obj.rect.height);

			cv::rectangle(image, obj.rect, cv::Scalar(255, 0, 0));

			char text[256];
			sprintf(text, "%s %.1f%%", class_names[obj.label], obj.prob * 100);

			int baseLine = 0;
			cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

			int x = obj.rect.x;
			int y = obj.rect.y - label_size.height - baseLine;
			if (y < 0)
				y = 0;
			if (x + label_size.width > image.cols)
				x = image.cols - label_size.width;

			cv::rectangle(image, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
				cv::Scalar(255, 255, 255), -1);

			cv::putText(image, text, cv::Point(x, y + label_size.height),
				cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
		}
		
		//cv::imwrite("ncnn_images.png", image);
		cv::imshow("image", image);

		cv::waitKey(0);
		//cv::destroyAllWindows();
	}

}