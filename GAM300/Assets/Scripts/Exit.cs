using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Exit : Script
{
    public GameObject noButton;
    public GameObject yesButton; 
    public GameObject yesTextObj; 
    public GameObject noTextObj;
    public FreeLookCamera camera;

    private SpriteRenderer noButtonRenderer;
    private SpriteRenderer yesButtonRenderer; 
    private TextRenderer yesText; 
    private TextRenderer noText;

    private float yestimer = 0f;
    private float notimer = 0f;
    private float duration = 0.3f;

    private vec3 initialColor;
    private vec3 finalColor;

    void Awake()
    {
        yesText = yesTextObj.GetComponent<TextRenderer>();
        noText = noTextObj.GetComponent<TextRenderer>();
        noButtonRenderer = noButton.GetComponent<SpriteRenderer>();
        yesButtonRenderer = yesButton.GetComponent<SpriteRenderer>();
    }

    void Start()
    {
        initialColor = yesText.color.xyz;
        finalColor = yesText.color.xyz * 4f;
    }

    void Update()
    {
        // Get refto Button
        if (noButtonRenderer != null)
        {
            if(noButtonRenderer.IsButtonClicked())
            {
                camera.GoToMainMenu();
            }
            
            if (noButtonRenderer.IsButtonHovered())
            {
                if (notimer < duration)
                {
                    notimer += Time.unscaledDeltaTime;
                    if (notimer > duration)
                    {
                        notimer = duration;
                    }
                    noText.color.xyz = vec3.Lerp(noText.color.xyz, finalColor, notimer / duration);
                }
            }
            else
            {
                if (notimer > 0f)
                {
                    notimer -= Time.unscaledDeltaTime;
                    if (duration < 0f)
                    {
                        duration = 0f;
                    }
                    noText.color.xyz = vec3.Lerp(noText.color.xyz, initialColor, notimer / duration);
                }
            }
        }

        if (yesButtonRenderer != null)
        {
            if (yesButtonRenderer.IsButtonClicked())
            {
                // Pan camera to black
                Application.Quit();
            }

            if (yesButtonRenderer.IsButtonHovered())
            {
                if (yestimer < duration)
                {
                    yestimer += Time.unscaledDeltaTime;
                    if (yestimer > duration)
                    {
                        yestimer = duration;
                    }
                    yesText.color.xyz = vec3.Lerp(yesText.color.xyz, finalColor, yestimer / duration);
                }
            }
            else
            {
                if (yestimer > 0f)
                {
                    yestimer -= Time.unscaledDeltaTime;
                    if (duration < 0f)
                    {
                        duration = 0f;
                    }
                    yesText.color.xyz = vec3.Lerp(yesText.color.xyz, initialColor, yestimer / duration);
                }
            }
        }
    }

   


}
