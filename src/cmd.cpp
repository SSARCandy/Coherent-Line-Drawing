#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include "CLD.h"
#include "postProcessing.h"

int main(int argc, char *argv[])
{
    std::string src_path, output_path;
    int ETF_iter = 1, CLD_iter = 1, ETF_kernel = 5;

    // clang-format off
    boost::program_options::options_description description("Coherent-Line-Drawing Options");
    description.add_options()
        ("help,h", "Help message")
        ("src,s", boost::program_options::value<std::string>(&src_path)->required(), "Source image path")
        ("output,o", boost::program_options::value<std::string>(&output_path)->required(), "Output image path")
        ("ETF_kernel", boost::program_options::value<int>(&ETF_kernel), "ETF kernel size, default kernel size = 5")
        ("ETF_iter", boost::program_options::value<int>(&ETF_iter), "Refining n times ETF, default is 1 iteration")
        ("CLD_iter", boost::program_options::value<int>(&CLD_iter), "Iterate n times FDoG, default is 1 iteration");
    // clang-format on

    try
    {
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vm);

        if (vm.count("help"))
        {
            std::cout << description << std::endl;
            return 0;
        }
        boost::program_options::notify(vm);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::cout << "Source image path = " << src_path << std::endl;
    std::cout << "Output image path = " << output_path << std::endl;
    std::cout << "ETF kernel size = " << ETF_kernel << std::endl;
    std::cout << "ETF iteration = " << ETF_iter << std::endl;
    std::cout << "CLD iteration = " << CLD_iter << std::endl;

    CLD cld;
    cld.readSrc(src_path);

    // Performing ETF refinement
    for (int i = 0; i < ETF_iter; ++i)
    {
        cld.etf.refine_ETF(ETF_kernel);
    }

    // Generate coherent line drawing
    cld.genCLD();

    // Refining line drawing by iterative FDoG
    for (int i = 0; i < CLD_iter; ++i)
    {
        cld.combineImage();
        cld.genCLD();
    }

    cv::cvtColor(cld.result, cld.result, CV_GRAY2RGB);
    cv::imwrite(output_path, cld.result);
}