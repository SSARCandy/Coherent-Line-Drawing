#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include "include/CLD.h"
#include "include/postProcessing.h"

void print_parameters(const boost::program_options::variables_map &vm)
{
    // clang-format off
    std::cout << "Source image path = " << vm["src"].as<std::string>() << std::endl;
    std::cout << "Output image path = " << vm["output"].as<std::string>() << std::endl;
    std::cout << "Generate post processed images = " << vm["debug_img"].as<bool>() << std::endl;

    std::cout << "[ETF] Parameters\n"
              << "  kernel size = " << vm["ETF_kernel"].as<int>() << "\n"
              << "  iteration = " << vm["ETF_iter"].as<int>() << "\n"
              << std::endl;

    std::cout << "[CLD] Parameters\n"
              << "  iteration = " << vm["CLD_iter"].as<int>() << "\n"
              << "  sigma_c = " << vm["sigma_c"].as<float>() << "\n"
              << "  sigma_m = " << vm["sigma_m"].as<float>() << "\n"
              << "  rho = " << vm["rho"].as<float>() << "\n"
              << "  tau = " << vm["tau"].as<float>() << "\n"
              << std::endl;
    // clang-format on
}

int main(int argc, char *argv[])
{
    std::string src_path, output_path;
    int ETF_iter, CLD_iter, ETF_kernel;
    float sigma_c, sigma_m, rho, tau;
    bool generate_post_processed_images{false};

    // clang-format off
    boost::program_options::options_description description("Coherent-Line-Drawing Options");
    description.add_options()
        ("help,h", "Help message")
        ("src,s", boost::program_options::value<std::string>(&src_path)->required(), "Source image path")
        ("output,o", boost::program_options::value<std::string>(&output_path)->required(), "Output image path")
        ("ETF_kernel", boost::program_options::value<int>(&ETF_kernel)->default_value(5), "ETF kernel size")
        ("ETF_iter", boost::program_options::value<int>(&ETF_iter)->default_value(1), "Refining n times ETF")
        ("CLD_iter", boost::program_options::value<int>(&CLD_iter)->default_value(1), "Iterate n times FDoG")
        ("sigma_c", boost::program_options::value<float>(&sigma_c)->default_value(1.0), "Line width")
        ("sigma_m", boost::program_options::value<float>(&sigma_m)->default_value(3.0), "Degree of coherence")
        ("rho", boost::program_options::value<float>(&rho)->default_value(0.997), "Noise")
        ("tau", boost::program_options::value<float>(&tau)->default_value(0.8), "Thresholding")
        ("debug_img", boost::program_options::bool_switch(&generate_post_processed_images), "Generate post processed images (anti-alias, flowfield visualize)");
    // clang-format on

    try {
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vm);

        if (vm.count("help")) {
            std::cout << description << std::endl;
            return 0;
        }
        boost::program_options::notify(vm);
        print_parameters(vm);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    CLD cld;
    cld.sigma_c = sigma_c;
    cld.sigma_m = sigma_m;
    cld.tau     = tau;
    cld.rho     = rho;
    cld.readSrc(src_path);

    // Performing ETF refinement
    for (int i = 0; i < ETF_iter; ++i) {
        cld.etf.refine_ETF(ETF_kernel);
    }

    // Generate coherent line drawing
    cld.genCLD();

    // Refining line drawing by iterative FDoG
    for (int i = 0; i < CLD_iter; ++i) {
        cld.combineImage();
        cld.genCLD();
    }

    cv::cvtColor(cld.result, cld.result, CV_GRAY2RGB);
    cv::imwrite(output_path, cld.result);
    std::cout << "Result image save at " << output_path << std::endl;

    if (generate_post_processed_images) {
        // Generate flow-field visualization
        cv::Mat vis_etf = postprocess::visualizeETF(cld.etf.flowField);
        vis_etf.convertTo(vis_etf, CV_8UC1, 255);
        cv::cvtColor(vis_etf, vis_etf, CV_GRAY2BGR);
        cv::imwrite("visualize-etf.jpg", vis_etf);

        // Generate flow-field arrow direction visualization
        cv::Mat vis_flow = postprocess::visualizeFlowfield(cld.etf.flowField);
        vis_flow.convertTo(vis_flow, CV_8UC3, 255);
        cv::cvtColor(vis_flow, vis_flow, CV_RGB2BGR);
        cv::imwrite("arrow-etf.jpg", vis_flow);

        // Generate Anti-alias coherent line drawing
        cv::Mat anti_alias = postprocess::antiAlias(cld.result.clone());
        cv::imwrite("anti-alias.jpg", anti_alias);
    }

    return 0;
}