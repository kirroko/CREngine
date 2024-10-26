#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>

class Animation
{
public:
    int totalFrames;
    int currentFrame;
    float frameDuration;
    float elapsedTime;
    int frameWidth;
    int frameHeight;
    int totalWidth;
    int totalHeight;

    Animation() = default;

    // Constructor to initialize an animation
    Animation(int totalFrames, int frameWidth, int frameHeight, int totalWidth, int totalHeight, float frameDuration)
        : totalFrames(totalFrames), currentFrame(0), frameDuration(frameDuration), elapsedTime(0.0f),
        frameWidth(frameWidth), frameHeight(frameHeight), totalWidth(totalWidth), totalHeight(totalHeight){}

    // Update animation frame based on elapsed time
    void update(float deltaTime);

};

#endif // ANIMATION_H
