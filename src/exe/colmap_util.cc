#include "exe/colmap_util.h"
#include "util/misc.h"
#include "util/option_manager.h"
#include "ui/main_window.h"
#include "base/camera_models.h"
//#include "QtWidgets/qapplication.h"
#include "feature/extraction.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif

namespace colmap {
    int RunSimpleFeatureExtractor(int argc, char** argv, std::vector<FeatureDescriptors>& query) {
#ifdef CUDA_ENABLED
        const bool kUseOpenGL = false;
#else
        const bool kUseOpenGL = true;
#endif

        std::string image_list_path;

        OptionManager options;
        options.AddDatabaseOptions();
        options.AddImageOptions();
        options.AddDefaultOption("image_list_path", &image_list_path);
        options.AddExtractionOptions();
        options.Parse(argc, argv);


        ImageReaderOptions reader_options = *options.image_reader;

        //custom option that disable database transaction when set camera parameters
        reader_options.query_image = true;
        reader_options.database_path = *options.database_path;
        reader_options.image_path = *options.image_path;

        if (!image_list_path.empty()) {
            reader_options.image_list = ReadTextFileLines(image_list_path);
            if (reader_options.image_list.empty()) {
                return EXIT_SUCCESS;
            }
        }

        if (!ExistsCameraModelWithName(options.image_reader->camera_model)) {
            std::cerr << "ERROR: Camera model does not exist" << std::endl;
        }

        std::unique_ptr<QApplication> app;
        if (options.sift_extraction->use_gpu && kUseOpenGL) {
            app.reset(new QApplication(argc, argv));
        }

        SiftFeatureSimpleExtractor feature_extractor(reader_options,
            *options.sift_extraction);

        if (options.sift_extraction->use_gpu && kUseOpenGL) {
            RunThreadWithOpenGLContext(&feature_extractor);
        }
        else {
            feature_extractor.Start();
            feature_extractor.Wait();
        }

        auto result = feature_extractor.getResult()->query_descriptors;
        for (int i = 0; i < result.size(); i++) {
            query.push_back(*result[i].get());
        }
        return EXIT_SUCCESS;
    }
}