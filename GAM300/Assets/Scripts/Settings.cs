using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Settings : Script
{
    public GameObject backButton;

    public GameObject masterVolumeButton;

    public GameObject bgmVolumeButton;

    public GameObject sfxVolumeButton;

    public GameObject gammaBtn;

    public GameObject gammaSettings;

    public GameObject gammaValue;

    public GameObject plus;

    public GameObject minus;

    public GameObject gammaBackButton;


    public bool isStartActive = true;
    public float flickerTimer = 0f;

    //movement variables
    public float duration = 2f;
    public float timer = 0f;
    public bool back = false;
    public bool test = false;

    public float sizeMultiplier = 1.5f;

    //sounds
    public AudioSource bgm;
    //public AudioSource uibutton;

    private SpriteRenderer backButtonRenderer;
    private SpriteRenderer masterVolumeButtonRenderer;
    private SpriteRenderer bgmVolumeButtonRenderer;
    private SpriteRenderer sfxVolumeButtonRenderer;
    private SpriteRenderer gammaButtonRenderer;
    private SpriteRenderer plusButtonRenderer;
    private SpriteRenderer minusButtonRenderer;
    private SpriteRenderer gammaBackButtonRenderer;
    private TextRenderer gammaText;


    //vec3 startGridTextSize;

    void Start()
    {
        //bgm.Play();
        //startGridTextSize = new vec3(mainMenuTitle.transform.localScale);
        //currentRestTimer = restTimer;

        bgm.Play();
        test = true;


        if (backButton.HasComponent<SpriteRenderer>())
            backButtonRenderer = backButton.GetComponent<SpriteRenderer>();
        if (masterVolumeButton.HasComponent<SpriteRenderer>())
            masterVolumeButtonRenderer = masterVolumeButton.GetComponent<SpriteRenderer>();
        if (bgmVolumeButton.HasComponent<SpriteRenderer>())
            bgmVolumeButtonRenderer = bgmVolumeButton.GetComponent<SpriteRenderer>();
        if (sfxVolumeButton.HasComponent<SpriteRenderer>())
            sfxVolumeButtonRenderer = sfxVolumeButton.GetComponent<SpriteRenderer>();
        if (gammaBtn.HasComponent<SpriteRenderer>())
            gammaButtonRenderer = gammaBtn.GetComponent<SpriteRenderer>();
        if (plus.HasComponent<SpriteRenderer>())
            plusButtonRenderer = plus.GetComponent<SpriteRenderer>();
        if (minus.HasComponent<SpriteRenderer>())
            minusButtonRenderer = minus.GetComponent<SpriteRenderer>();
        if (gammaBackButton.HasComponent<SpriteRenderer>())
            gammaBackButtonRenderer = gammaBackButton.GetComponent<SpriteRenderer>();
        if (gammaValue.HasComponent<TextRenderer>())
            gammaText = gammaValue.GetComponent<TextRenderer>();
    }

    void Update()
    {
        if(gammaButtonRenderer != null && gammaButtonRenderer.IsButtonClicked())
        {
            gammaSettings.SetActive(true);
            gammaBtn.SetActive(false);
        }

        if (gammaBackButtonRenderer != null && gammaBackButtonRenderer.IsButtonClicked())
        {
            gammaSettings.SetActive(false);
            gammaBtn.SetActive(true);
        }

        if (plusButtonRenderer != null && gammaButtonRenderer.IsButtonClicked())
        {
            Graphics.gamma += 0.1f;
            gammaText.text = Graphics.gamma.ToString();
        }

        // Get refto Button
        if (minusButtonRenderer != null && backButtonRenderer.IsButtonClicked())
        {
            Graphics.gamma -= 0.1f;
            gammaText.text = Graphics.gamma.ToString();
        }

        if (masterVolumeButtonRenderer != null && masterVolumeButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Music");
            //InternalCalls.PauseMusic();
            //bgm.Pause();
            //SceneManager.LoadScene("MainMenu");
            if(test == true)
            {
                bgm.Pause();
                test = false;
            }
            else{
                bgm.Play();
                test = true;

            }
        }
        if (bgmVolumeButtonRenderer != null && bgmVolumeButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Music");
            //InternalCalls.PauseMusic();
            //bgm.Pause();
            //SceneManager.LoadScene("MainMenu");
            if(test == true)
            {
                bgm.Pause();
                test = false;
            }
            else{
                bgm.Play();
                test = true;

            }
        }
        if (sfxVolumeButtonRenderer != null && sfxVolumeButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Music");
            //InternalCalls.PauseMusic();
            //bgm.Pause();
            //SceneManager.LoadScene("MainMenu");
            if(test == true)
            {
                bgm.Pause();
                test = false;
            }
            else{
                bgm.Play();
                test = true;

            }
        }
   


        // Check if button state is clicked

        // Do something(Load)
        //goToPlay();

        //code not working atm
        //selfFlicker();
       // movement();
    }

   


}
