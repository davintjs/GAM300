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
    public FadeEffect fader;

    private SpriteRenderer noButtonRenderer;
    private SpriteRenderer yesButtonRenderer;
    private bool waitingExit = false;

    void Awake()
    {
        noButtonRenderer = noButton.GetComponent<SpriteRenderer>();
        yesButtonRenderer = yesButton.GetComponent<SpriteRenderer>();
    }

    void Start()
    {

    }

    void Update()
    {
        // Get refto Button
        if (noButtonRenderer != null && noButtonRenderer.IsButtonClicked())
        {
            camera.GoToMainMenu();
        }

        if (yesButtonRenderer != null && yesButtonRenderer.IsButtonClicked())
        {
            camera.GoToFinalExit();

            fader.StartFadeIn(2.5f, true, 0f, 1f);
            waitingExit = true;
        }

        // Pan camera to black
        if (waitingExit && fader.finished)
        {
            Application.Quit();
        }
    }

   


}
