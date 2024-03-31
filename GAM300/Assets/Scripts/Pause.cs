using System;
using System.Collections;
using System.Linq;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Pause : Script
{
    public GameObject PauseMenuObj;
    public GameObject HTPMenuObj;
    public GameObject OptionsMenuObj;
    public GameObject ExitMenuObj;

    public SpriteRenderer ResumeRenderer;
    public SpriteRenderer HTPRenderer;
    public SpriteRenderer MainMenuRenderer;
    public SpriteRenderer OptionsRenderer;
    public SpriteRenderer ExitRenderer;
    public SpriteRenderer NoRenderer;
    public FadeEffect fader;

    public int currentMenu = 0;
    private bool waitingMainMenu = false;

    void Start()
    {
        HTPMenuObj.SetActive(false);
        OptionsMenuObj.SetActive(false);
        ExitMenuObj.SetActive(false);
    }

    void Update()
    {
        if (ResumeRenderer != null && ResumeRenderer.IsButtonClicked())
        {
            GameManager.instance.paused = false;
        }

        if (HTPRenderer != null && HTPRenderer.IsButtonClicked())
        {
            StartCoroutine(Wait(1));
        }

        if (MainMenuRenderer != null && MainMenuRenderer.IsButtonClicked())
        {
            fader.StartFadeIn(1f, true, 0f, 1f);
            waitingMainMenu = true;
        }

        if(waitingMainMenu && fader.finished)
        {
            SceneManager.LoadScene("MainMenu", true);
        }    

        if (OptionsRenderer != null && OptionsRenderer.IsButtonClicked())
        {
            StartCoroutine(Wait(2));
        }

        if (ExitRenderer != null && ExitRenderer.IsButtonClicked())
        {
            StartCoroutine(Wait(3));
        }

        if (NoRenderer != null && NoRenderer.IsButtonClicked())
        {
            StartCoroutine(Wait(0));
        }

        if (Input.GetKey(KeyCode.Escape))
        {
            if (currentMenu != 0)
            {
                StartCoroutine(Wait(0));
            }
        }
    }

    void SelectMenu(int menu)
    {
        currentMenu = menu;

        PauseMenuObj.SetActive(false);
        HTPMenuObj.SetActive(false);
        OptionsMenuObj.SetActive(false);
        ExitMenuObj.SetActive(false);

        switch (currentMenu)
        {
            case 0:
                PauseMenuObj.SetActive(true);
                break;
            case 1:
                HTPMenuObj.SetActive(true);
                break;
            case 2:
                OptionsMenuObj.SetActive(true);
                break;
            case 3:
                ExitMenuObj.SetActive(true);
                break;
            default:
                break;
        }
    }

    IEnumerator Wait(int menu)
    {
        SelectMenu(menu);
        yield return new WaitForSeconds(0.1f);
    }
}
