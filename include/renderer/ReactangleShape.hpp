#ifndef REACTANGLESHAPE_HPP
#define REACTANGLESHAPE_HPP

//Simple 2D rectangle
class ReactangleShape
{
    public:
        float width, height, x, y, r, g, b;

        ReactangleShape();
        void setSize(float width, float height);
        void setPosition(float x, float y);
        void setColor(int r, int g, int b); 
};

#endif
