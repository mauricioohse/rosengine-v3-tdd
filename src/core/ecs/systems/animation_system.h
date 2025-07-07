
#include "../engine.h"

class AnimationSystem {
public:
    void Update(float deltaTime) {
        auto view = g_Engine.entityManager.GetEntitiesWith<AnimationComponent>();
        
        for (auto entity : view) {
            auto& anim = g_Engine.entityManager.GetComponent<AnimationComponent>(entity);
            
            if (!anim.playing) continue;
            
            anim.accumulator += deltaTime;
            
            if (anim.accumulator >= anim.frameTime) {
                anim.accumulator -= anim.frameTime;
                
                anim.currentFrame++;
                if (anim.currentFrame >= anim.totalFrames) {
                    if (anim.loop) {
                        anim.currentFrame = 0;
                    } else {
                        anim.currentFrame = anim.totalFrames - 1;
                        anim.playing = false;
                    }
                }
                
                anim.UpdateFrameRect();
            }
        }
    }
}; 