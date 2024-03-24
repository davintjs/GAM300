using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class TitleScreen : Script
{
    public GameObject gameLogoObj;
    public GameObject pressGameObj;
    public GameObject copyrightGameObj;

    public MainMenu mainMenu;
    public AudioSource spaceButton;

    private SpriteRenderer gameLogo;
    private SpriteRenderer copyright;
    private TextRenderer pressText;

    private float timer = 0f;
    private float duration = 1f;
    private bool lockFadeOut = false;

    private Coroutine fadeIn;

    void Awake()
    {
        gameLogo = gameLogoObj.GetComponent<SpriteRenderer>();
        copyright = copyrightGameObj.GetComponent<SpriteRenderer>();
        pressText = pressGameObj.GetComponent<TextRenderer>();
    }

    void Start()
    {
        gameLogoObj.SetActive(false);
        copyrightGameObj.SetActive(false);
        pressGameObj.SetActive(false);

        //float data = InstanceData.GetData("TitleScreen");
        //Console.WriteLine("Data: " + data);
        fadeIn = StartCoroutine(FadeIn());
        //if(data == 0)
        //{
        //}
    }

    void Update()
    {
        if(Input.GetKeyDown(KeyCode.Space) && !lockFadeOut)
        {
            //InstanceData.SaveData("TitleScreen", 1f); // Save data that title screen has already been loaded
            StopCoroutine(fadeIn);
            spaceButton.Play();
            StartCoroutine(FadeOut());
        }
    }

    IEnumerator FadeIn()
    {
        yield return new WaitForSeconds(1f);

        float alpha = gameLogo.alpha;
        gameLogo.alpha = 0f;
        gameLogoObj.SetActive(true);

        while (gameLogo.alpha < alpha) // Fade the game logo in
        {
            gameLogo.alpha = Mathf.Lerp(0f, alpha, timer, duration, Mathf.EasingType.EASEIN);

            timer += Time.deltaTime;
            yield return null;
        }

        timer = 0f;
        copyright.alpha = 0f;
        copyrightGameObj.SetActive(true);
        pressText.color.a = 0f;
        pressGameObj.SetActive(true);

        while (timer < duration)
        {
            copyright.alpha = Mathf.Lerp(0f, 1f, timer, duration, Mathf.EasingType.EASEIN);
            pressText.color.a = Mathf.Lerp(0f, 1f, timer, duration, Mathf.EasingType.EASEIN);

            timer += Time.deltaTime;
            yield return null;
        }

        yield return new WaitForSeconds(1f);

        timer = 0f;
        bool toggle = false;

        while(!lockFadeOut)
        {
            if(toggle)
                pressText.color.a = Mathf.Lerp(0.2f, 1f, timer, duration, Mathf.EasingType.BEZIER);
            else
                pressText.color.a = Mathf.Lerp(1f, 0.2f, timer, duration, Mathf.EasingType.BEZIER);

            if(timer > duration)
            {
                timer = 0f;
                toggle = !toggle;
            }

            timer += Time.deltaTime;
            yield return null;
        }
    }

    IEnumerator FadeOut()
    {
        lockFadeOut = true;

        timer = 0f;
        float alpha = pressText.color.a;
        float logoAlpha = gameLogo.alpha;
        while (timer < duration)
        {
            copyright.alpha = Mathf.Lerp(1f, 0f, timer, duration, Mathf.EasingType.EASEOUT);
            gameLogo.alpha = Mathf.Lerp(logoAlpha, 0f, timer, duration, Mathf.EasingType.EASEOUT);
            pressText.color.a = Mathf.Lerp(alpha, 0f, timer, duration, Mathf.EasingType.EASEOUT);

            timer += Time.deltaTime;
            yield return null;
        }

        mainMenu.InitializeMainMenu();
    }
}
