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

    public GameObject pressStartText;

    public GameObject mainMenuTitle;

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

    vec3 startGridTextSize;

    void Start()
    {
        bgm.Play();
        startGridTextSize = new vec3(mainMenuTitle.transform.localScale);
        //currentRestTimer = restTimer;
    }

    void Update()
    {
        goToPlay();

        //code not working atm
        selfFlicker();
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

    void selfFlicker()
    {
        flickerTimer += Time.deltaTime;

        if (flickerTimer > duration)
        {
            flickerTimer = 0f;
            switch (isStartActive)
            {
                case true:
                    pressStartText.SetActive(false);
                    isStartActive = false;
                    break;
                case false:
                    pressStartText.SetActive(true);
                    isStartActive = true;
                    break;
            }

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
