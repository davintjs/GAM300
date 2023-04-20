#include "precompiled.h"

//#include "Mesh.h"
//
//void Mesh::Draw(xgpu::cmd_buffer& CmdBuffer)
//{
//    CmdBuffer.setBuffer(m_VertexBuffer);
//    CmdBuffer.setBuffer(m_IndexBuffer);
//    CmdBuffer.Draw(m_IndexBuffer.getEntryCount());
//}
//
//xgpu::device::error* Mesh::setupMesh(void)
//{
//    // Setup vertex buffer
//    xgpu::buffer::setup Setup
//    { .m_Type = xgpu::buffer::type::VERTEX
//    , .m_Usage = xgpu::buffer::setup::usage::GPU_READ
//    , .m_EntryByteSize = sizeof(Vertex)
//    , .m_EntryCount = static_cast<int>(mVertices.size())
//    , .m_pData = mVertices.data()
//    };
//    if (auto Err = mDevice.Create(m_VertexBuffer, Setup); Err)
//    {
//        return Err;
//    }
//
//    // Setup index buffer
//    xgpu::buffer::setup Setup2
//    { .m_Type = xgpu::buffer::type::INDEX
//    , .m_Usage = xgpu::buffer::setup::usage::GPU_READ
//    , .m_EntryByteSize = sizeof(int)
//    , .m_EntryCount = static_cast<int>(mIndices.size())
//    , .m_pData = mIndices.data()
//    };
//    if (auto Err = mDevice.Create(m_IndexBuffer, Setup2); Err)
//    {
//        return Err;
//    }
//
//    return nullptr;
//}