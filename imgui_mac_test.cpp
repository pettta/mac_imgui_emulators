#include <stdio.h> 

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <imgui.h>
#include <imgui_impl_metal.h>


/** Basic Metal Window Begin*/
class Renderer
{
    public:
        Renderer( MTL::Device* pDevice );
        ~Renderer();
        void buildShaders();
        void buildBuffers();

    private:
        MTL::Device* _pDevice;
        MTL::CommandQueue* _pCommandQueue;
        MTL::Library* _pShaderLibrary;
        MTL::RenderPipelineState* _pPSO;
        MTL::Buffer* _pArgBuffer;
        MTL::Buffer* _pVertexPositionsBuffer;
        MTL::Buffer* _pVertexColorsBuffer;
};

Renderer::Renderer( MTL::Device* pDevice )
: _pDevice( pDevice->retain() )
{
    _pCommandQueue = _pDevice->newCommandQueue();
}

Renderer::~Renderer()
{
    _pCommandQueue->release();
    _pDevice->release();
}


/** Basic Metal Window End **/

/** Imgui Wrapper of Metal Functionality Begin */
class ImguiAppDelegate : public NS::Object
{
    public:
        ~ImguiAppDelegate();
        void draw( MTK::View* pView );
        void boot( NS::Window* pWindow, MTL::Device* pDevice)

    private:
        NS::Window* _pWindow;
        MTL::Device* _pDevice;
        Renderer* _pRenderer;

};

ImguiAppDelegate::~ImguiAppDelegate(){
    _pWindow->release();
    _pDevice->release();
    ImGui_ImplMetal_Shutdown();
    ImGui::DestroyContext();
}

MTK::View* _pMktView ImguiAppDelegate::boot()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup graphics from metal   
    CGRect frame = (CGRect){ {100.0, 100.0}, {512.0, 512.0} };
    _pWindow = NS::Window::alloc()->init(
        frame,
        NS::WindowStyleMaskClosable|NS::WindowStyleMaskTitled,
        NS::BackingStoreBuffered,
        false );
    if (_pWindow == nullptr)
    {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }

    _pDevice = MTL::CreateSystemDefaultDevice();

    _pMktView = MTK::View::alloc()->initWithFrame_device( frame, _pDevice );
    _pMktView->setColorPixelFormat(MTL::PixelFormat::BGRA8Unorm_sRGB);
    _pMktView->setClearColor( MTL::ClearColor::Make(0.45f, 0.55f, 0.60f, 1.00f) );

    _pRenderer = new Renderer( _pDevice );
    _pMktView->setDelegate(_pRenderer);
    _pWindow->setContentView( _pMktView );
    _pWindow->makeKeyAndOrderFront( nullptr );

}

void ImguiAppDelegate::draw( MTK::View* pView )
{
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );

    // Start ImGui frame
    ImGui_ImplMetal_NewFrame(pRpd);
    ImGui::NewFrame();

    //Show ImGui demo window
    ImGui::ShowDemoWindow(&true);

    // Rendering
    ImGui::Render();
    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), pCmd, pEnc);

    // Cleanup
    pEnc->popDebugGroup();
    pEnc->endEncoding();
    pCmd->presentDrawable( pView->currentDrawable() );
    pCmd->commit();
}


/** Imgui Wrapper of Metal Functionality End **/

int main(int argc, char* argv[]){
    NS:: AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    // Imgui starts up metal stuff 
    ImguiAppDelegate* pAppDelegate = new ImguiAppDelegate();
    MTK::View* view = pAppDelegate->boot();
    pAppDelegate->draw(view);
    // Cleanup
    pPool->release();
    return 0 ;
}

