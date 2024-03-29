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
    public GameObject startButton;
    public GameObject settingsButton; 
    public GameObject HTPButton; 
    public GameObject ExitButton;
    //public GameObject mainMenuObj;

    public GameObject spotLightObj;
    public GameObject incubatorObj;
    public FadeEffect fader;

    public FreeLookCamera camera;

    public AudioSource bgm;

    private SpriteRenderer startButtonRenderer;
    private SpriteRenderer settingsButtonRenderer;
    private SpriteRenderer HTPButtonRenderer;
    private SpriteRenderer ExitButtonRenderer;
    //private SpriteRenderer mainMenuRenderer;

    private LightSource spotLight;
    private MeshRenderer incubatorGlass;
    private float timer = 0f;
    private float duration = 2f;
    private float alpha = 0.11f;
    private bool waitingPlay = false;

    void Awake()
    {
        spotLight = spotLightObj.GetComponent<LightSource>();
        incubatorGlass = incubatorObj.GetComponent<MeshRenderer>();
        //mainMenuRenderer = mainMenuObj.GetComponent<SpriteRenderer>();

        startButtonRenderer = startButton.GetComponent<SpriteRenderer>();
        settingsButtonRenderer = settingsButton.GetComponent<SpriteRenderer>();
        HTPButtonRenderer = HTPButton.GetComponent<SpriteRenderer>();
        ExitButtonRenderer = ExitButton.GetComponent<SpriteRenderer>();

    }

    void Start()
    {
        vec4 color = incubatorGlass.material.color;
        color.a = 1f;
        spotLightObj.SetActive(false);
        incubatorGlass.material.SetRawColor(color);

        startButton.SetActive(false);
        HTPButton.SetActive(false);
        settingsButton.SetActive(false);
        ExitButton.SetActive(false);

        bgm.Play();
    }

    void Update()
    {
        // Get refto Button
        if (startButton.activeSelf && startButtonRenderer.IsButtonClicked())
        {
            camera.GoToPlay();

            fader.StartFadeIn(2f, true, 0f, 1f);
            waitingPlay = true;
        }

        if(waitingPlay && fader.finished)
        {
            float currentLevel = InstanceData.GetData("CurrentLevel");
            Action loadScene = () => SceneManager.LoadScene("StartingScene", true);
            switch (currentLevel)
            {
                case 1f:
                    loadScene = () => SceneManager.LoadScene("LevelTutorial", true);
                    break;
                case 2f:
                    loadScene = () => SceneManager.LoadScene("LevelPlay2", true);
                    break;
                case 3f:
                    loadScene = () => SceneManager.LoadScene("LevelBoss", true);
                    break;
                default:
                    break;
            }
            StartCoroutine(QueueAction(loadScene));
        }

        if(Input.GetKeyDown(KeyCode.Escape)) 
        {
            camera.GoToMainMenu();
        }

        if (settingsButton.activeSelf && settingsButtonRenderer.IsButtonClicked())
        {
            camera.GoToSettings();
        }

        if (HTPButton.activeSelf && HTPButtonRenderer.IsButtonClicked())
        {
            camera.GoToHTP();
        }

        if (ExitButton.activeSelf && ExitButtonRenderer.IsButtonClicked())
        {
            camera.GoToExit();
        }

    }

    public void InitializeMainMenu()
    {
        StartCoroutine(InitializeSceneObjects());
    }

    IEnumerator InitializeSceneObjects()
    {
        yield return new WaitForSeconds(0.5f);

        vec4 oldColor = incubatorGlass.material.color;
        float intensity = spotLight.intensity;
        float glassAlpha = 1f;
        spotLight.intensity = 0f;
        spotLightObj.SetActive(true);
        duration = 2f;

        while (timer < duration)
        {
            spotLight.intensity = Mathf.Lerp(0f, intensity, timer, 1f, Mathf.EasingType.LINEAR);
            glassAlpha = Mathf.Lerp(1f, alpha, timer, duration, Mathf.EasingType.BEZIER);
            incubatorGlass.material.SetRawColor(new vec4(oldColor.xyz, glassAlpha));

            timer += Time.deltaTime;
            yield return null;
        }

        StartCoroutine(FadeIn(startButtonRenderer, startButton, 0f));
        StartCoroutine(FadeIn(HTPButtonRenderer, HTPButton, 0.1f));
        StartCoroutine(FadeIn(settingsButtonRenderer, settingsButton, 0.2f));
        StartCoroutine(FadeIn(ExitButtonRenderer, ExitButton, 0.3f));
    }

    IEnumerator FadeIn(SpriteRenderer renderer, GameObject spriteObj, float wait)
    {
        float tempTimer = 0f;
        float tempDuration = 0.5f;
        renderer.alpha = 0f;
        spriteObj.SetActive(true);

        yield return new WaitForSeconds(wait);

        while (tempTimer < tempDuration)
        {
            renderer.alpha = Mathf.Lerp(0f, 1f, tempTimer, tempDuration, Mathf.EasingType.BEZIER);
            tempTimer += Time.deltaTime;
            yield return null;
        }
    }

    IEnumerator QueueAction(Action action)
    {
        yield return new WaitForSeconds(0.2f);
        action();
    }
}
