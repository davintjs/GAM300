using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class MainMenu : Script
{
    public GameObject mainMenuBGImage;

    public GameObject mainMenuTitle;

    public GameObject startButton;

    public GameObject settingsButton; 

    public GameObject HTPButton; 

    public GameObject ExitButton; 



    public bool isStartActive = true;
    public float flickerTimer = 0f;

    //movement variables
    public float duration = 2f;
    public float timer = 0f;
    public bool back = false;

    public float sizeMultiplier = 1.5f;

    //sounds
    public AudioSource bgm;
    public AudioSource uibutton;

    private SpriteRenderer startButtonRenderer;

    private SpriteRenderer settingsButtonRenderer;

    private SpriteRenderer HTPButtonRenderer;

    private SpriteRenderer ExitButtonRenderer;


    vec3 startGridTextSize;

    void Start()
    {
        bgm.Play();
        startGridTextSize = new vec3(mainMenuTitle.transform.localScale);
        //currentRestTimer = restTimer;

        //Play Button
        if (startButton.HasComponent<SpriteRenderer>())
        {
            startButtonRenderer = startButton.GetComponent<SpriteRenderer>();
        }
        //Settings Button
        if (settingsButton.HasComponent<SpriteRenderer>())
        {
            settingsButtonRenderer = settingsButton.GetComponent<SpriteRenderer>();
        }
        //HTP Button
        if (HTPButton.HasComponent<SpriteRenderer>())
        {
            HTPButtonRenderer = HTPButton.GetComponent<SpriteRenderer>();
        }
        //Exit Button
        if (ExitButton.HasComponent<SpriteRenderer>())
        {
            ExitButtonRenderer = ExitButton.GetComponent<SpriteRenderer>();
        }


            
    }

    void Update()
    {
        // Get refto Button
        if (startButtonRenderer != null && startButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Scene");
            SceneManager.LoadScene("StartingScene", true);

        }

        if (settingsButtonRenderer != null && settingsButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Scene");
            SceneManager.LoadScene("Settings", true);

        }

        if (HTPButtonRenderer != null && HTPButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Scene");
            SceneManager.LoadScene("HowToPlay",true);

        }

        if (ExitButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Scene");
            SceneManager.LoadScene("Exit", true);

        }

        // Check if button state is clicked

        // Do something(Load)
        goToPlay();

        //code not working atm
        //selfFlicker();
        movement();
    }

    IEnumerator LoadScene(float seconds)
    {
        yield return new WaitForSeconds(seconds);
        SceneManager.LoadScene("LevelPlay2");
    }

    void goToPlay()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            uibutton.Play();
            StartCoroutine(LoadScene(0.1f));
        }
    }


    void movement()
    {
        if (!back)
        {
            mainMenuTitle.transform.localScale = vec3.Lerp(startGridTextSize, startGridTextSize * sizeMultiplier, timer / duration);
        }
        else
        {
            mainMenuTitle.transform.localScale = vec3.Lerp(startGridTextSize*sizeMultiplier, startGridTextSize, timer / duration);
        }
        timer += Time.deltaTime;
        if (timer >= duration)
        {
            timer = 0f;
            back = !back;
        }
    }

}
