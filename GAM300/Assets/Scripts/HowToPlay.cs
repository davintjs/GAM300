using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class HowToPlay : Script
{
    public GameObject mainMenuBGImage;

    public GameObject backButton;

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

    private SpriteRenderer backButtonRenderer;

    vec3 startGridTextSize;

    void Start()
    {
        //bgm.Play();
        //startGridTextSize = new vec3(mainMenuTitle.transform.localScale);
        //currentRestTimer = restTimer;

        if (backButton.HasComponent<SpriteRenderer>())
            backButtonRenderer = backButton.GetComponent<SpriteRenderer>();
    }

    void Update()
    {
        // Get refto Button
        if (backButtonRenderer != null && backButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Scene");
            SceneManager.LoadScene("MainMenu", true);

        }

        // Check if button state is clicked

        // Do something(Load)
        //goToPlay();

        //code not working atm
        //selfFlicker();
       // movement();
    }

   


}
