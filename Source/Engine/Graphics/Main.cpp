#include "Renderer.h"

int main() {
    Renderer renderer;

    //renderer.init();
    //renderer.render();

    //renderer.drawBox(800.0f, 450.0f, 400.f, 250.f, false);
    //renderer.drawBox(0.0f, 0.0f, 400.f, 250.f, true);

    renderer.drawCircle(800.0f, 100.0f, 200.f, true, 100);

    return 0;
}