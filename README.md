# tgter
A small header-only C++ library for reading and writing Terragen TER files.

The Terragen TER file format is documented here:

https://planetside.co.uk/wiki/index.php?title=Terragen_.TER_Format


# Example of Usage

{

    #include \<string\>
    #include "tgterread.h"

    {
        std::string filename = "test.ter";

        TgTerHeader header(0, 0);

        // read header (readmode 0)
        ResultOf_ReadTgTerFile result = ReadTgTerFile(filename.c_str(), 0, &header, nullptr, nullptr);

        if (result.succeeded)
        {
            // allocate array (bad habits from C, sorry!)
            float* altitudes = new float[header.pointsX * header.pointsY];

            // fill array (readmode 1)
            TgTerAlts destination(altitudes, 1, header.scaleM, 1.0f / header.scaleM);
            result = ReadTgTerFile(filename.c_str(), 1, &header, &destination, nullptr);

            // do your thing here
            //

            // free memory
            delete [] altitudes;
        }
    }
}
