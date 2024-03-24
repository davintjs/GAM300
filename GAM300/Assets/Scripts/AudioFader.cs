using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class AudioFader : Script
{
    private AudioSource audioSource;

    private float startVal = 0f;
    private float duration = 0f;
    public bool isFading = false;

    public GameObject fader;

    public float inDuration = 1f;
    public float outDuration = 1f;
    public float waitDelay = 0f;
    public float targetVal = 1f;

    public bool finished = false;
    public bool looping = false;

    private Coroutine fadeInCoroutine;
    private Coroutine fadeOutCoroutine;
    private Coroutine fadeInAndOutCoroutine;

    void Awake()
    {
        if (fader.HasComponent<AudioSource>())
            audioSource = fader.GetComponent<AudioSource>();
    }

    //public void StartFadeInAndOut(float fadeDuration = 1f)
    //{
    //    isFading = true;
    //    finished = false;
    //    duration = inDuration = outDuration = fadeDuration;
    //    startVal = 0.0f;
    //    targetVal = 1.0f;
    //    if (fader != null && audioSource != null)
    //    {
    //        audioSource. = startVal;
    //        fader.SetActive(true);
    //        fadeInAndOutCoroutine = StartCoroutine(FadeInAndOut());
    //    }
    //}

    //public void StartFadeIn(float fadeDuration = 1f, bool keepActive = false, float inDelay = 0f, float outDelay = 0f)
    //{
    //    isFading = true;
    //    finished = false;
    //    startVal = 0.0f;
    //    targetVal = 1.0f;
    //    duration = inDuration = fadeDuration;

    //    if (fader != null && spriteRenderer != null)
    //    {
    //        spriteRenderer.alpha = startVal;
    //        fader.SetActive(true);
    //        fadeInCoroutine = StartCoroutine(FadeInOrOut(keepActive, inDelay, outDelay));
    //    }
    //}

    //IEnumerator FadeInOrOut()
    //{

    //}
}