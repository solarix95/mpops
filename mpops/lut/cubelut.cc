#include <iostream>
#include <sstream>
#include "lut/cubelut.h"

//----------------------------------------------------------------------------
CubeLUT::LUTState CubeLUT::LoadCubeFile(ifstream &infile)
{
    // Set defaults
    status = OK;
    title.clear();
    domainMin = tableRow ( 3, 0.0 );
    domainMax = tableRow ( 3, 1.0 );
    LUT1D.clear();
    LUT3D.clear();

    //Read file data line by line
    const char NewlineCharacter = '\n';
    char lineSeparator = NewlineCharacter;
    // sniff use of legacy lineSeparator
    const char CarriageReturnCharacter = '\r';
    for (int i = 0; i < 255; i++) {
        char inc = infile.get();
        if ( inc == NewlineCharacter ) break;
        if ( inc == CarriageReturnCharacter ) {
            if ( infile.get() == NewlineCharacter ) break;
            lineSeparator = CarriageReturnCharacter;
            std::cerr << "INFO: This file uses non-compliant line separator \\r (0x0D)" << endl;
            break;
        }
        if ( i > 250 ) { status = LineError; break; }
    }
    infile.seekg ( 0 );
    infile.clear();

    // read keywords
    int N, CntTitle, CntSize, CntMin, CntMax;

    // each keyword to occur zero or one time
    N = CntTitle = CntSize = CntMin = CntMax = 0;

    while ( status == OK ) {
        long linePos = infile.tellg();
        string lineOfText = ReadLine ( infile, lineSeparator );
        if ( ! status == OK ) break;

        // Parse keywords and parameters
        istringstream line ( lineOfText );
        string keyword;
        line >> keyword;
        if ( "+" < keyword && keyword < ":" ) {
            // lines of table data come after keywords
            // restore stream pos to re-read line of data
            infile.seekg ( linePos );
            break;
        } else if ( keyword == "TITLE" && CntTitle++ == 0 ) {
            const char
                    QUOTE = '"';
            char startOfTitle;
            line >> startOfTitle;
            if ( startOfTitle != QUOTE ) { status = TitleMissingQuote; break; }
            getline ( line, title, QUOTE );
            // read to "
        } else if ( keyword == "DOMAIN_MIN" && CntMin++ == 0 ) {
            line >> domainMin[0] >> domainMin[1] >> domainMin[2];
        } else if ( keyword == "DOMAIN_MAX" && CntMax++ == 0 ) {
            line >> domainMax[0] >> domainMax[1] >> domainMax[2];
        } else if ( keyword == "LUT_1D_SIZE" && CntSize++ == 0 ) {
            line >> N;
            if ( N < 2 || N > 65536 ) { status = LUTSizeOutOfRange; break; }
            LUT1D = table1D ( N, tableRow ( 3 ) );
        } else if ( keyword == "LUT_3D_SIZE" && CntSize++ == 0 ) {
            line >> N;
            if ( N < 2 || N > 256 ) { status = LUTSizeOutOfRange; break; }
            LUT3D = table3D ( N, table2D ( N, table1D ( N, tableRow ( 3 ) ) ) );
        } else { status = UnknownOrRepeatedKeyword; break; }
        if ( line.fail() ) { status = ReadError; break; }
    } //read keywords

    if ( status == OK && CntSize == 0 ) status = LUTSizeOutOfRange;
    if ( status == OK && ( domainMin[0] >= domainMax[0] || domainMin[1] >= domainMax[1]
                           || domainMin[2] >= domainMax[2] ) )
        status = DomainBoundsReversed;

    // read lines of table data
    if ( LUT1D.size() > 0 ) {
        N = LUT1D.size();
        for ( int i = 0; i < N && status == OK; i++ ) {
            LUT1D [i] = ParseTableRow ( ReadLine ( infile, lineSeparator ) );
        }
    } else {
        N = LUT3D.size();
        // NOTE that r loops fastest
        for ( int b = 0; b < N && status == OK; b++ ) {
            for ( int g = 0; g < N && status == OK; g++ ) {
                for ( int r = 0; r < N && status == OK; r++ ) {
                    LUT3D[r][g][b] = ParseTableRow
                            ( ReadLine ( infile, lineSeparator ) );
                }
            }
        }
    }
    return status;
}

//-----------------------------------------------------------------------------
CubeLUT::LUTState CubeLUT::SaveCubeFile(ofstream &outfile)
{
    if ( ! status == OK ) return status; // Write only good Cubes

    // Write keywords
    const char SPACE = ' ';
    const char QUOTE = '"';

    if ( title.size() > 0 ) outfile << "TITLE" << SPACE << QUOTE << title << QUOTE << endl;
    outfile << "# Created by CubeLUT.cc" << endl;
    outfile << "DOMAIN_MIN" << SPACE << domainMin[0] << SPACE << domainMin[1]
            << SPACE << domainMin[2] << endl;
    outfile << "DOMAIN_MAX" << SPACE << domainMax[0] << SPACE << domainMax[1]
            << SPACE << domainMax[2] << endl;

    // Write LUT data
    if ( LUT1D.size() > 0 ) {
        int N = LUT1D.size();
        outfile << "LUT_1D_SIZE" << SPACE << N << endl;
        for ( int i = 0; i < N && outfile.good(); i++ ) {
            outfile << LUT1D[i] [0] << SPACE << LUT1D[i] [1] << SPACE << LUT1D[i] [2] << endl;
        }
    } else {
        int N = LUT3D.size();
        outfile << "LUT_3D_SIZE" << SPACE << N << endl;

        // NOTE that r loops fastest

        for ( int b = 0; b < N && outfile.good(); b++ ) {
            for ( int g = 0; g < N && outfile.good(); g++ ) {
                for ( int r = 0; r < N && outfile.good(); r++ ) {
                    outfile << LUT3D[r][g][b] [0] << SPACE << LUT3D[r][g][b] [1]
                            << SPACE << LUT3D[r][g][b] [2] << endl;
                }
            }
        }
    }
    outfile.flush();
    return ( outfile.good() ? OK : WriteError );
}

//-----------------------------------------------------------------------------
void CubeLUT::initSelfTest()
{
    status    = OK;
    title     = "selftest";
    domainMin.clear();
    domainMin.push_back(0.0);
    domainMin.push_back(0.0);
    domainMin.push_back(0.0);
    domainMax.clear();
    domainMax.push_back(0.0);
    domainMax.push_back(0.0);
    domainMax.push_back(0.0);
    domainMax[0] = 1;

    tableRow cell;  cell.push_back(0);cell.push_back(0);cell.push_back(0);
    table1D  blue;  blue.push_back(cell);  blue.push_back(cell);
    table2D  green; green.push_back(blue); green.push_back(blue);
    table3D  red;   red.push_back(green);  red.push_back(green);
    LUT3D = red;
    LUT3D[0][0][0][0] = 0; LUT3D[0][0][0][1] = 0; LUT3D[0][0][0][2] = 0;
    LUT3D[1][0][0][0] = 1; LUT3D[1][0][0][1] = 0; LUT3D[1][0][0][2] = 0;
    LUT3D[0][1][0][0] = 0; LUT3D[0][1][0][1] = 1; LUT3D[0][1][0][2] = 0;
    LUT3D[1][1][0][0] = 1; LUT3D[1][1][0][1] = 1; LUT3D[1][1][0][2] = 0;
    LUT3D[0][0][1][0] = 0; LUT3D[0][0][1][1] = 0; LUT3D[0][0][1][2] = 1;
    LUT3D[1][0][1][0] = 1; LUT3D[1][0][1][1] = 0; LUT3D[1][0][1][2] = 1;
    LUT3D[0][1][1][0] = 0; LUT3D[0][1][1][1] = 1; LUT3D[0][1][1][2] = 1;
    LUT3D[1][1][1][0] = 1; LUT3D[1][1][1][1] = 1; LUT3D[1][1][1][2] = 1;
}

//-----------------------------------------------------------------------------
string CubeLUT::ReadLine ( ifstream & infile, char lineSeparator )
{
    // Skip empty lines and comments
    const char CommentMarker = '#';
    string textLine("");
    while ( textLine.size() == 0 || textLine[0] == CommentMarker ) {
        if ( infile.eof() ) {
            status = PrematureEndOfFile;
            break;
        }
        getline ( infile, textLine, lineSeparator );
        if ( infile.fail() ) {
            status = ReadError; break;
        }
    }
    return textLine;
}

//-----------------------------------------------------------------------------
CubeLUT::tableRow CubeLUT::ParseTableRow(const string &lineOfText)
{
    int N = 3;
    tableRow f ( N );
    istringstream line ( lineOfText );
    for (int i = 0; i < N; i++) {
        line >> f[i];
        if ( line.fail() ) { status = CouldNotParseTableData; break; };
    }
    return f;
}
