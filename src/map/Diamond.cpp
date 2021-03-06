

#include "map/Diamond.h"
#include <type_traits>

Diamond::Diamond(unsigned int size,
                 const Matrix<float> &northMap,
                 const Matrix<float> &eastMap,
                 const Matrix<float> &southMap,
                 const Matrix<float> &westMap)
        : _northMapEmpty(northMap.empty()), _eastMapEmpty(eastMap.empty()),
          _southMapEmpty(southMap.empty()), _westMapEmpty(westMap.empty())
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    _gen = std::mt19937(rd());

    _size = size;
    _map = Matrix<float>(_size, _size, 0.f);

    // init map corners
    _map.at(0, 0) = boundedRand(-_height, _height);
    _map.at(_map.rows() - 1, _map.cols() - 1) = boundedRand(-_height, _height);
    _map.at(_map.rows() - 1, 0) = boundedRand(-_height, _height);
    _map.at(0, _map.cols() - 1) = boundedRand(-_height, _height);

    // If this chunk is surrounded by other chunks
    // we use the neighbours to init the sides of the map
    if (!_westMapEmpty)
        for (int i = 0; i < westMap.cols(); i++)
            _map.at(0, i) = westMap.at(_size - 1, i);

    if (!_eastMapEmpty)
        for (int i = 0; i < eastMap.cols(); i++)
            _map.at(_size - 1, i) = eastMap.at(0, i);

    if (!_northMapEmpty)
        for (int i = 0; i < northMap.rows(); i++)
            _map.at(i, _size - 1) = northMap.at(i, 0);

    if (!_southMapEmpty)
        for (int i = 0; i < southMap.rows(); i++)
            _map.at(i, 0) = southMap.at(i, _size - 1);
}

void    Diamond::manageSquare(unsigned int x, unsigned int y, const unsigned int size, const std::shared_ptr<Biome> &biome)
{
    _map.at(x + size / 2, y + size / 2) =
            ((_map.at(x, y) + _map.at(x + size, y) +
              _map.at(x, y + size) + _map.at(x + size, y + size)) / 4.f)
            + biome->boundedRand();
}

void    Diamond::manageDiamond(unsigned int x, unsigned int y, const unsigned int size, int flag, const std::shared_ptr<Biome> &biome)
{
    float topDiamond = 0.f;
    float leftDiamond = 0.f;
    float topDivider = 3.f;
    float leftDivider = 3.f;

    if (_southMapEmpty || y >= size / 2)
    {
        if (y >= size / 2)
        {
            topDiamond = _map.at(x + size / 2, y - size / 2);
            topDivider++;
        }
        _map.at(x + size / 2, y) = ((_map.at(x, y) + _map.at(x + size, y) +
                                     _map.at(x + size / 2, y + size / 2) +
                                     topDiamond) / topDivider) + biome->boundedRand();
    }
    if (_westMapEmpty || x >= size / 2)
    {
        if (x >= size / 2)
        {
            leftDiamond = _map.at(x - size / 2, y + size / 2);
            leftDivider++;
        }
        _map.at(x, y + size / 2) = ((_map.at(x, y) + _map.at(x, y + size) +
                                     _map.at(x + size / 2, y + size / 2) +
                                     leftDiamond) / leftDivider) + biome->boundedRand();
    }
    if (flag & RIGHT_END && _eastMapEmpty)
    {
        _map.at(x + size, y + size / 2) = ((_map.at(x + size, y + size) + _map.at(x + size, y) +
                                            _map.at(x + size / 2, y + size / 2)) / 3.f) + biome->boundedRand();
    }
    if (flag & TOP_END && _northMapEmpty)
    {
        _map.at(x + size / 2, y + size) = ((_map.at(x + size, y + size) + _map.at(x, y + size) +
                                            _map.at(x + size / 2, y + size / 2)) / 3.f) + biome->boundedRand();
    }
}

void    Diamond::fillMap()
{
    unsigned int    nbSquare = (_size - 1) / 2;
    unsigned int    squareSize = _size - 1;
    /*_map.at(squareSize / 2, squareSize / 2) = (_map.at(0, 0) +
                                              _map.at(squareSize, squareSize) +
                                              _map.at(squareSize, 0) +
                                              _map.at(0, squareSize) +
                                              _map.at(squareSize / 2, 0) +
                                              _map.at(squareSize / 2, squareSize) +
                                              _map.at(0, squareSize / 2) +
                                              _map.at(squareSize, squareSize / 2)) / 8.f;*/

    Matrix<std::shared_ptr<Biome>> biomes(2, 2, nullptr);
    biomes.at(0, 0) = getBiome(_map.at(0,0), _map.at(0, squareSize / 2), _map.at(squareSize / 2, 0), _map.at(squareSize / 2, squareSize / 2));
    biomes.at(1, 0) = getBiome(_map.at(squareSize / 2, 0), _map.at(squareSize, 0), _map.at(squareSize / 2, squareSize / 2), _map.at(squareSize, squareSize / 2));
    biomes.at(0, 1) = getBiome(_map.at(0, squareSize / 2), _map.at(squareSize / 2, squareSize / 2), _map.at(0, squareSize), _map.at(squareSize / 2, squareSize));
    biomes.at(1, 1) = getBiome(_map.at(squareSize / 2, squareSize / 2), _map.at(squareSize, squareSize / 2), _map.at(squareSize / 2, squareSize), _map.at(squareSize, squareSize));
    for (const auto &biome : biomes)
    {
        if (_nbMountain > 0 && dynamic_cast<Mountain *>(biome.get()))
            _height = biome->_height;
        else if (_nbMountain < 1 && dynamic_cast<Land *>(biome.get()))
            _height = biome->_height;
    }

    for (int i = 1; i <= nbSquare; i *= 2)
    {
        int line = 0;
        int column = 1;
        for (int j = 0; j < i * i; ++j)
        {
            if (i > 1) {
                line = (j / i) / (i / 2);
                column = (j % i) / (i / 2);
                _height = biomes.at(line, column)->_height;
            }
            manageSquare(j % i * squareSize, j / i * squareSize,
                         squareSize, biomes.at(line, column));// check if end of column + check if end of line
            manageDiamond(j % i * squareSize, j / i * squareSize, squareSize,
                          (2 * (j / i == i - 1)) + (j % i == i - 1), biomes.at(line, column));
        }
        squareSize /= 2;

        if (i > 1)
        {
            for (auto &line : biomes) {
                if (dynamic_cast<Mountain *>(line.get()))
                    line->_height /= 3.f;
                else
                    line->_height /= 2.f;
            }
        }
        else
        {
            for (auto &line : biomes) {
                if (dynamic_cast<Mountain *>(line.get()))
                    line->_height /= 1.5f;
                else
                    line->_height /= 1.5f;
            }
        }
    }
}

float   Diamond::getDiff(int x, int y, unsigned int size) const
{
    float max = _map.at(size, size);
    float min = _map.at(size, size);

    for (int i = x * size; i <= x * size + size; i++)
    {
        for (int j = y * size; j <= y * size + size; j++)
        {
            max = std::max(_map.at(i, j), max);
            min = std::min(_map.at(i, j), min);
        }
    }
    std::cout << "diff " << max - min << std::endl;
    return max - min;
}

float	Diamond::boundedRand(float min, float max)
{
    std::uniform_real_distribution<> dis(min, max);
    return (static_cast<float>(dis(_gen)));
}

void	Diamond::printMap() const
{
    for (const auto &value : _map)
    {
        std::cout << value << ", " << std::endl;;
    }
}

std::shared_ptr<Biome>  Diamond::getBiome(float A, float B, float C, float D)
{
    float height = (A + B + C + D) / 4.f;
    if (height > 15.f)
    {
        std::cout << "mountain" << std::endl;
        _nbMountain++;
        return (std::make_shared<Mountain>());
    }
    else
    {
        std::cout << "land" << std::endl;
        return (std::make_shared<Land>());
    }
}