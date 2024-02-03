#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_metal.h>
#include <stdio.h> 

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

static void glfw3_errorCallback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


int main()
{

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup style to be dark 
    ImGui::StyleColorsDark();

    // Initialize GLFW
    glfwSetErrorCallback(glfw3_errorCallback);
    if (!glfwInit() )
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }
    
    // Create window via graphics context 
    GLFWwindow* window = glfwCreateWindow(800, 600, "ImGui Test", nullptr, nullptr);
    if (window == nullptr)
    {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }

    //glfwMakeContextCurrent(window); TODO REMOVE 
    
    // Initialize Metal
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    id <MTLCommandQueue> commandQueue = [device newCommandQueue];
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplMetal_Init(device);

    // Use GLFW to get native window handle
    NSWindow *nsWindow = glfwGetCocoaWindow(window);
    CA::MetalLayer* pMetalLayer;
    pMetalLayer.device = device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    nsWindow.contentView.layer = layer; 
    nsWindow.contentView.wantsLayer = YES;

    MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::new();

    // State for rendering
    bool show_demo_window = true;
    bool show_another_window = false;
    float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        @autoreleasepool{
            glfwPollEvents();
            // Start the Dear ImGui frame via metal 
            int width, height; 
            glfwGetFramebufferSize(window, &width, &height);
            layer.drawableSize = CGSizeMake(width, height);
            CA:MetalDrawable* drawable = [layer nextDrawable];

            id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clear_color[0] * clear_color[3], clear_color[1] * clear_color[3], clear_color[2] * clear_color[3], clear_color[3]);
            renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
            [renderEncoder pushDebugGroup:@"ImGui demo"];
            
            // Start ImGui frame
            ImGui_ImplMetal_NewFrame(renderPassDescriptor);
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // ImGui code goes here
            if (show_demo_window)
                ImGui::ShowDemoWindow(&show_demo_window);

            // Rendering
            ImGui::Render();
            ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

            [renderEncoder popDebugGroup];
            [renderEncoder endEncoding];

            [commandBuffer presentDrawable:drawable];
            [commandBuffer commit];

        }
        
    }

    // Cleanup
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
