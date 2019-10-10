#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include "CLD.h"
#include "postProcessing.h"

int main(int argc, char *argv[])
{
    std::string src_path;
    std::string output_path;
    int ETF_iter = 1, CLD_iter = 1;

    // clang-format off
    boost::program_options::options_description description("Coherent-Line-Drawing Options");
    description.add_options()
        ("help,h", "Help message")
        ("src,s", boost::program_options::value<std::string>(&src_path)->required(), "Source image path")
        ("output,o", boost::program_options::value<std::string>(&output_path)->required(), "Output image path")
        ("ETF_iter", boost::program_options::value<int>(&ETF_iter), "Refining n times ETF")
        ("CLD_iter", boost::program_options::value<int>(&CLD_iter), "Iterate n times FDoG");
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

    std::cout << "source image path = " << src_path << std::endl;
    std::cout << "output image path = " << output_path << std::endl;
    std::cout << "ETF iteration = " << ETF_iter << std::endl;
    std::cout << "CLD iteration = " << CLD_iter << std::endl;

    CLD cld;
    cld.readSrc(src_path);

    for (int i = 0; i < ETF_iter; ++i)
    {
        cld.etf.refine_ETF(5);
    }

    cld.genCLD();
    for (int i = 0; i < CLD_iter; ++i)
    {
        cld.combineImage();
        cld.genCLD();
    }

    cv::cvtColor(cld.result, cld.result, CV_GRAY2RGB);
    // cv::imshow("t", cld.result);
    // cv::waitKey(0);
    cv::imwrite(output_path, cld.result);
}