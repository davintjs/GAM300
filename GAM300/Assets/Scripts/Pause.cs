using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Pause : Script
{
    public GameObject PauseImage;

    public GameObject pauseTitle;

    public GameObject resumeButton;

    public GameObject HTPButton; 

    public GameObject MainMenu; 

    public bool isStartActive = true;
    public float flickerTimer = 0f;

    //movement variables
    public float duration = 2f;
    public float timer = 0f;
    public bool back = false;

    public float sizeMultiplier = 1.5f;


    private SpriteRenderer resumeButtonRenderer;

    private SpriteRenderer HTPButtonRenderer;

    private SpriteRenderer MainMenuRenderer;


    void Start()
    {
        //Play Button
        if (resumeButton.HasComponent<SpriteRenderer>())
        {
            resumeButtonRenderer = resumeButton.GetComponent<SpriteRenderer>();
        }
        //HTP Button
        if (HTPButton.HasComponent<SpriteRenderer>())
        {
            HTPButtonRenderer = HTPButton.GetComponent<SpriteRenderer>();
        }
        //Exit Button
        if (MainMenu.HasComponent<SpriteRenderer>())
        {
            MainMenuRenderer = MainMenu.GetComponent<SpriteRenderer>();
        }
            
    }

    void Update()
    {
        // Get refto Button
        if (resumeButtonRenderer != null && resumeButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Scene");
            SceneManager.LoadScene("LevelTutorial");

        }

        if (HTPButtonRenderer != null && HTPButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Scene");
            SceneManager.LoadScene("HowToPlay",true);

        }

        if (MainMenuRenderer != null && MainMenuRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Scene");
            SceneManager.LoadScene("MainMenu", true);

        }

    }


}
