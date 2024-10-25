#include "Animation.h"

void Animation::update(float deltaTime)
{
    elapsedTime += deltaTime;
    if (elapsedTime >= frameDuration)
    {
        currentFrame++;
        if (currentFrame >= totalFrames)
        {
            currentFrame = 0;
        }
        elapsedTime = 0.0f;
    }
}
