#include "raylib.h"
#include "vector"

using namespace std;

float brushRadius = 5.0f;

struct Layer{
    vector<vector<Vector2>> strokes;
    bool visible = true;
    void Draw(){
        for(auto& stroke : strokes){
            for(int i=1; i<stroke.size(); i++){
                DrawLineEx(stroke[i-1], stroke[i], brushRadius*2.5, BLACK);
                DrawCircle(stroke[i].x, stroke[i].y, brushRadius, BLACK);
            }
        }
    }
};

vector<Layer> layers;
vector<Vector2> stroke;
int currentLayer = 0;
float undoTimer = 0.0f;
bool saveRequested = false;

int main(){
    layers.push_back(Layer());
    InitWindow(800, 450, "Inny's Paint App");
    SetTargetFPS(60);
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            stroke.clear();
            // 클릭한 레이어탭 색 변경
            Vector2 mousePos = GetMousePosition();
            if(mousePos.x >=650){
                for(int i=0; i<layers.size(); i++){
                    int layerY = 450-(50*(i+1));
                    if(mousePos.y >= layerY && mousePos.y < layerY+50)
                        currentLayer = i;
                }
            }
        }
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            Vector2 mousePos = GetMousePosition();
            if(mousePos.x < 650 && layers[currentLayer].visible)
                stroke.push_back(mousePos);
        }
        if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
            layers[currentLayer].strokes.push_back(stroke);
        }
        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z)){
            if(!layers[currentLayer].strokes.empty())
                layers[currentLayer].strokes.pop_back();
            else undoTimer = 2.0f;
        }
        if(IsKeyPressed(KEY_N)){
            layers.push_back(Layer());
            currentLayer = layers.size() - 1;
        }
        if(IsKeyPressed(KEY_V)){
            if(layers[currentLayer].visible)
                layers[currentLayer].visible = false;
            else layers[currentLayer].visible = true;
        }
        if(IsKeyDown(KEY_ONE) && layers.size()>=1){
            currentLayer = 0;
            DrawText("Layer 1", 10, 30, 10, BLACK);
        }
        if(IsKeyDown(KEY_TWO) && layers.size()>=2){
            currentLayer = 1;
            DrawText("Layer 2", 10, 30, 10, BLACK);
        }
        if(IsKeyPressed(KEY_BACKSPACE) && layers.size()>1){
            layers.erase(layers.begin()+currentLayer);
            if(currentLayer >= layers.size())
                currentLayer = layers.size() - 1;
        }
        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)){
            saveRequested = true;
        }
        for(auto& layer : layers){
            if(layer.visible)
                layer.Draw();
        }
        if(undoTimer > 0){
            DrawText("Can't do undo.", 10, 410, 30, BLACK);
            undoTimer -= GetFrameTime();
        }

        // 레이어 표시
        DrawRectangle(650, 0, 150, 450, LIGHTGRAY);
        for(int i=0; i<layers.size(); i++){
            Color rectColor = (i==currentLayer) ? DARKGRAY : GRAY;
            DrawRectangle(650, 450-(50*(i+1)), 150, 50, rectColor);
            Color textColor = (i==currentLayer) ? YELLOW : WHITE;
            auto onOff = "ON";
            if(!layers[i].visible) { textColor = LIGHTGRAY; onOff = "OFF"; }
            DrawText(TextFormat("Layer %i", i+1), 660, 450-(50*(i+1)), 15, textColor);
            DrawText(TextFormat("%s", onOff), 750, 450-(50*(i+1)), 15, textColor);
        }

        /* 디버깅
        DrawText(TextFormat("Num of layers: %i",layers.size()), 
                10, 10, 10, DARKBLUE);
        DrawText(TextFormat("Current Layer is: %i", currentLayer+1),
                10, 20, 10, DARKGREEN);
        */

        EndDrawing();

        if(saveRequested){
            Image image = LoadImageFromScreen();
            Rectangle cropRect = {0, 0, 1300, 900};
            ImageCrop(&image, cropRect);
            ExportImage(image, "output.png");
            UnloadImage(image);
            saveRequested = false;
        }
    }
    CloseWindow();
    return 0;
}