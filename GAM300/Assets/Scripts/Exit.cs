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
    public GameObject mainMenuBGImage;

    public GameObject noButton;

    public GameObject yesButton; 

    public bool isStartActive = true;
    public float flickerTimer = 0f;

    //movement variables
    public float duration = 2f;
    public float timer = 0f;
    public bool back = false;

    public float sizeMultiplier = 1.5f;

    //sounds
    //public AudioSource bgm;
    //public AudioSource uibutton;

    private SpriteRenderer noButtonRenderer;
    private SpriteRenderer yesButtonRenderer; 

    //vec3 startGridTextSize;

    void Start()
    {
        //bgm.Play();
        //startGridTextSize = new vec3(mainMenuTitle.transform.localScale);
        //currentRestTimer = restTimer;

        if (noButton.HasComponent<SpriteRenderer>())
            noButtonRenderer = noButton.GetComponent<SpriteRenderer>();

        if (yesButton.HasComponent<SpriteRenderer>())
            yesButtonRenderer = yesButton.GetComponent<SpriteRenderer>();

    }

    void Update()
    {
        // Get refto Button
        if (noButtonRenderer != null && noButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Scene");
            SceneManager.LoadScene("MainMenu", true);
        }

        if (yesButtonRenderer != null && yesButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Scene");
            //Application.Quit();

        }
    }

   


}
