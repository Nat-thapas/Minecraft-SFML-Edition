#ifndef MC_PROGRESSBAR_HPP
#define MC_PROGRESSBAR_HPP

#include <SFML/Graphics.hpp>

namespace mc {

class ProgressBar : public sf::Drawable, public sf::Transformable {
    sf::VertexArray vertexArray;
    sf::Texture& texture;
    sf::Vector2i textureSize;
    bool isVertical;
    float progress;

    void initializeVertexArray();
    void updateVertexArray();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    ProgressBar(sf::Texture& texture, bool isVertical);
    void setProgress(float progress);
};

}  // namespace mc

#endif  // MC_PROGRESSBAR_HPP