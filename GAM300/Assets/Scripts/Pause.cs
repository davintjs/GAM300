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

    public GameObject HowToPlayBack;

    public GameObject HowToPlayPanel;
    public GameObject PausePanel;

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

    private SpriteRenderer HowToPlayBackRenderer;


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

        if (HowToPlayBack.HasComponent<SpriteRenderer>())
        {
            HowToPlayBackRenderer = HowToPlayBack.GetComponent<SpriteRenderer>();
        }

    }

    void Update()
    {
        // Get refto Button
        if (resumeButtonRenderer != null && resumeButtonRenderer.IsButtonClicked())
        {
            Action resume = () =>
            {
                GameManager.instance.paused = false;
            };
            StartCoroutine(QueueAction(resume));
        }

        if (HTPButtonRenderer != null && HTPButtonRenderer.IsButtonClicked())
        {
            Action htpPlay = () =>
            {
                HowToPlayPanel.SetActive(true);
                PausePanel.SetActive(false);
            };
            StartCoroutine(QueueAction(htpPlay));
        }

        if (MainMenuRenderer != null && MainMenuRenderer.IsButtonClicked())
        {
            Action loadMain = () =>
            {
                SceneManager.LoadScene("MainMenu", true);
            };
            StartCoroutine(QueueAction(loadMain));
        }

        if (HowToPlayBackRenderer != null && HowToPlayBackRenderer.IsButtonClicked())
        {
            Action closeHTP = () =>
            {
                HowToPlayPanel.SetActive(false);
                PausePanel.SetActive(true);
            };
            StartCoroutine(QueueAction(closeHTP));
        }

    }

    IEnumerator QueueAction (Action action)
    {
        yield return new WaitForSeconds(0.2f);
        action();
    }
}
