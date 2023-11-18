using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class VictoryMenu : Script
{
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
    public AudioSource uiButton;

    void Start()
    {
        bgm.Play();
        //currentRestTimer = restTimer;
    }

    void Update()
    {
        goToMenu();

        //code not working atm
        //selfFlicker();
        //movement();
    }

    void goToMenu()
    {
        if (Input.GetKeyDown(KeyCode.M))
        {
            uiButton.Play();
            SceneManager.LoadScene("LevelPlay2.scene");
        }

    }
}
