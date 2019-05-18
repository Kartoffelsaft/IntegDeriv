#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <vector>
#include <tuple>
#include <algorithm>
#include <math.h>

float inputFunction(float x)
{
    return sinf32(x) / x;
}

struct Program
{
    sf::RenderWindow *window;
    const int resX = 1280;
    const int resY = 720;
    const char* const name = "Integral & Derivative Viewer";

    static const float domain[2];
    static const float range[2];
    const float dx = 0.01;

    std::vector<std::tuple<float, float>> DtoR(float (*ifunc)(float))
    {
        std::vector<std::tuple<float, float>> outputPoints;
        outputPoints.reserve((size_t)((domain[1] - domain[0])/dx));

        for(auto x = domain[0]; x < domain[1]; x += dx)
        {
            outputPoints.push_back(std::make_tuple(x, ifunc(x)));
        }
        return outputPoints;
    }

    std::vector<std::tuple<float, float>> derivative(float (*ifunc)(float))
    {
        std::vector<std::tuple<float, float>> outputPoints;
        outputPoints.reserve((size_t)((domain[1] - domain[0])/dx));

        float prevY = ifunc(domain[0]);
        for(auto x = domain[0]; x < (domain[1] - dx); x += dx)
        {
            auto y = prevY;
            auto ny = ifunc(x + dx);
            auto dy = ny - y;
            outputPoints.push_back(std::make_tuple(x, dy/dx));
            prevY = ny;
        }

        return outputPoints;
    }

    std::vector<std::tuple<float, float>> integral(float (*ifunc)(float))
    {
        std::vector<std::tuple<float, float>> outputPoints;
        outputPoints.reserve((size_t)((domain[1] - domain[0])/dx));

        float total = 0;
        for(float x = -dx; x >= domain[0]; x -= dx)
        {
            auto y = ifunc(x);
            total -= y * dx;
            outputPoints.push_back(std::make_tuple(x, total));
        }
        total = 0;
        std::reverse(outputPoints.begin(), outputPoints.end());
        for (auto x = dx; x < domain[1]; x += dx)
        {
            auto y = ifunc(x);
            total += y * dx;
            outputPoints.push_back(std::make_tuple(x, total));
        }

        return outputPoints;
    }

    void renderGraph()
    {        
        window->clear(sf::Color::White);

        auto baseFnLine = tupleArrayToVertexArray(this->DtoR(inputFunction), sf::Color::Black);
        window->draw(baseFnLine);

        auto derivativeFnLine = tupleArrayToVertexArray(this->derivative(inputFunction), sf::Color::Blue);
        window->draw(derivativeFnLine);

        auto integralFnLine = tupleArrayToVertexArray(this->integral(inputFunction), sf::Color::Red);
        window->draw(integralFnLine);

        window->display();
    }

    sf::VertexArray tupleArrayToVertexArray(std::vector<std::tuple<float, float>> tuples, sf::Color color)
    { 
        auto scaleX = resX / (domain[1] - domain[0]);
        auto scaleY = resY / (range[1] - range[0]);
        auto originX = resX * (-domain[0])/(domain[1] - domain[0]);
        auto originY = resY * (range[1])/(range[1] - range[0]);

        sf::VertexArray output(sf::LineStrip);
        for(auto tup : tuples)
        {
            auto nx = std::get<0>(tup) * scaleX + originX;
            auto ny = std::get<1>(tup) * -scaleY + originY;

            output.append(sf::Vertex(sf::Vector2f(nx, ny), color));
        }
        return output;
    }

public:
    Program()
    {
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8;
        window = new sf::RenderWindow(sf::VideoMode(resX, resY), name, sf::Style::Default, settings);
    }

    bool loop()
    {
        this->renderGraph();

        sf::Event event;
        window->waitEvent(event);
        do
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window->close();
                break;
            
            default:
                break;
            }
        }
        while(window->pollEvent(event));
        return window->isOpen();
    }

    ~Program()
    {}
};

const float Program::domain[2] = {-50.0f, 50.0f};
const float Program::range[2] = {-2.0f, 2.0f};

int main()
{
    auto prgm = new Program();

    while(prgm->loop()){std::this_thread::sleep_for(std::chrono::milliseconds(300));}

    delete prgm;

    return 0;
}