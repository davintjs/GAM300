#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include "Scene/SceneManager.h"


void BLOOM_PBR::Init(unsigned int windowWidth, unsigned int windowHeight)
{
	mSrcViewportSize = glm::ivec2(windowWidth, windowHeight);
	mSrcViewportSizeFloat = glm::vec2((float)windowWidth, (float)windowHeight);

	// Framebuffer
	const unsigned int num_bloom_mips = 6; // TODO: Play around with this value
	bool status = FBOInit(windowWidth, windowHeight, num_bloom_mips);
	if (!status) {
		std::cerr << "Failed to initialize bloom FBO - cannot create bloom renderer!\n";
	}
}

void BLOOM_PBR::RenderBloomTexture(float filterRadius, BaseCamera& _camera, unsigned int& _vao, unsigned int& _vbo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	RenderDownsamples(FRAMEBUFFER.GetTextureID(_camera.GetFramebufferID(), _camera.GetBloomAttachment()),_vao, _vbo);
	RenderUpsamples(filterRadius, _vao, _vbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Restore viewport
	glViewport(0, 0, mSrcViewportSize.x, mSrcViewportSize.y);
}


void BLOOM_PBR::RenderDownsamples(unsigned int srcTexture,unsigned int& _vao, unsigned int& _vbo)
{
	const std::vector<bloomMip>& mipChain = MipChain();

	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::DOWN_SAMPLE);
	shader.Use();

	//mDownsampleShader->setVec2("srcResolution", mSrcViewportSizeFloat);
	GLint srcRes = glGetUniformLocation(shader.GetHandle(), "srcResolution");
	glUniform2fv(srcRes, 1, glm::value_ptr(mSrcViewportSizeFloat));


	GLint mipL = glGetUniformLocation(shader.GetHandle(), "mipLevel");
	if (mKarisAverageOnDownsample) {

		//mDownsampleShader->setInt("mipLevel", 0);
		glUniform1i(mipL, 0);
	
	}

	// Bind srcTexture (HDR color buffer) as initial texture input
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	// Progressively downsample through the mip chain
	for (int i = 0; i < (int)mipChain.size(); i++)
	{
		const bloomMip& mip = mipChain[i];
		glViewport(0, 0, (GLsizei)mip.size.x, (GLsizei)mip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, mip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		//renderQuad();
		renderQuad(_vao,_vbo);

		// Set current mip resolution as srcResolution for next iteration
		//mDownsampleShader->setVec2("srcResolution", mip.size);
		glUniform2fv(srcRes, 1, glm::value_ptr(mip.size));




		// Set current mip as texture input for next iteration
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		// Disable Karis average for consequent downsamples
		if (i == 0) 
		{
			//mDownsampleShader->setInt("mipLevel", 1); 
			glUniform1i(mipL, 1);

		}
	}
}

void BLOOM_PBR::RenderUpsamples(float filterRadius, unsigned int& _vao, unsigned int& _vbo)
{
	const std::vector<bloomMip>& mipChain = MipChain();

	//mUpsampleShader->use();
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UP_SAMPLE);
	shader.Use();


	//mUpsampleShader->setFloat("filterRadius", filterRadius);
	GLint filterR = glGetUniformLocation(shader.GetHandle(), "filterRadius");
	glUniform1f(filterR, filterRadius);


	// Enable additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	for (int i = (int)mipChain.size() - 1; i > 0; i--)
	{
		const bloomMip& mip = mipChain[i];
		const bloomMip& nextMip = mipChain[i - 1];

		// Bind viewport and texture from where to read
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		// Set framebuffer render target (we write to this texture)
		glViewport(0, 0, (GLsizei)nextMip.size.x, (GLsizei)nextMip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, nextMip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		renderQuad(_vao,_vbo);
	}

	// Disable additive blending
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	glUseProgram(0);
}

GLuint BLOOM_PBR::BloomTexture()
{
	return MipChain()[0].texture;
}


bool BLOOM_PBR::FBOInit(unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength)
{
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
	glm::ivec2 mipIntSize((int)windowWidth, (int)windowHeight);
	// Safety check
	if (windowWidth > (unsigned int)INT_MAX || windowHeight > (unsigned int)INT_MAX) {
		std::cerr << "Window size conversion overflow - cannot build bloom FBO!" << std::endl;
		return false;
	}

	for (GLuint i = 0; i < mipChainLength; i++)
	{
		bloomMip mip;

		mipSize *= 0.5f;
		mipIntSize /= 2;
		mip.size = mipSize;
		mip.intSize = mipIntSize;

		glGenTextures(1, &mip.texture);
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		// we are downscaling an HDR color buffer, so we need a float texture format
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
		             (int)mipSize.x, (int)mipSize.y,
		             0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		std::cout << "Created bloom mip " << mipIntSize.x << 'x' << mipIntSize.y << std::endl;
		mMipChain.emplace_back(mip);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	                       GL_TEXTURE_2D, mMipChain[0].texture, 0);

	// setup attachments
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	// check completion status
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("gbuffer FBO error, status: 0x%x\n", status);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;



}



