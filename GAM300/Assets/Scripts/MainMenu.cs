using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class MainMenu : Script
{
    public GameObject mainMenuBGImage;

    public GameObject pressStartText;

    public bool isStartActive = true;
    public float flickerTimer = 0f;

    //movement variables
    public float duration = 2f;
    public float timer = 0f;
    public bool back = false;

    public bool rest = false;
    public float restTimer = 2f;
    public float currentRestTimer;

    //sounds
    public AudioSource bgm;
    public AudioSource uibutton;

    void Start()
    {
        bgm.Play();
        //currentRestTimer = restTimer;
    }

    void Update()
    {
        goToPlay();

        //code not working atm
        //selfFlicker();
        //movement();
    }

    void goToPlay()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            uibutton.Play();
            SceneManager.LoadScene("LevelPlay2");
        }
    }

    void selfFlicker()
    {
        flickerTimer += Time.deltaTime;

        if (flickerTimer > 0.5f)
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
        if (!back && !rest)
        {
            mainMenuBGImage.transform.localPosition = vec3.Lerp(new vec3(-0.069f, 0f, 0f), new vec3(-0.072f, 0f, 0f), timer / duration);
        }
        else if (back && !rest)
        {
            mainMenuBGImage.transform.localPosition = vec3.Lerp(new vec3(-0.072f, 0f, 0f), new vec3(-0.069f, 0f, 0f), timer / duration);
        }
        timer += Time.deltaTime;
        if (timer >= duration)
        {
            rest = true;//stops platform from moving
            //Console.WriteLine("PlatformResting");
        }
        if (rest)
        {
            currentRestTimer -= Time.deltaTime;
            if (currentRestTimer <= 0)
            {
                //Console.WriteLine("PlatformStartMoving");
                rest = false;
                currentRestTimer = restTimer;//rest rest timer
                back = !back;//change to the other movement direction
                timer = 0f;//rest movement timer
            }
        }
    }

}
