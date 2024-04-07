using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;


public class FadeEffect : Script
{
    private SpriteRenderer spriteRenderer;

    private float startVal = 0f;
    private float duration = 0f;
    public bool isFading = false;

    public GameObject fader;
 
    public float inDuration = 1f;
    public float outDuration = 1f;
    public float waitDelay = 0f;
    public float stayDuration = 0.5f;
    public float targetVal = 1f;

    public bool finished = false;
    public bool looping = false;

    private Coroutine fadeInCoroutine;
    private Coroutine fadeOutCoroutine;
    private Coroutine fadeInAndOutCoroutine;

    void Awake()
    {
        if (fader.HasComponent<SpriteRenderer>())
            spriteRenderer = fader.GetComponent<SpriteRenderer>();
    }

    void Update()
    {
        if(looping && !isFading)
        {
            StartFadeInAndOut(inDuration);
        }
    }

    public void StartFadeInAndOut(float fadeDuration = 1f)
    {
        Reset();
        isFading = true;
        finished = false;
        duration = inDuration = outDuration = fadeDuration;
        startVal = 0.0f;
        targetVal = 1.0f;
        if (fader != null && spriteRenderer != null)
        {
            spriteRenderer.alpha = startVal;
            fader.SetActive(true);
            fadeInAndOutCoroutine = StartCoroutine(FadeInAndOut());
        }
    }
    
    public void StartFadeIn(float fadeDuration = 1f, bool keepActive = false, float inDelay = 0f, float outDelay = 0f)
    {
        Reset();
        isFading = true;
        finished = false;
        startVal = 0.0f;
        targetVal = 1.0f;
        duration = inDuration = fadeDuration;

        if(fader != null && spriteRenderer != null)
        {
            spriteRenderer.alpha = startVal;
            fader.SetActive(true);
            fadeInCoroutine = StartCoroutine(FadeInOrOut(keepActive, inDelay, outDelay));
        }
    }

    public void StartFadeOut(float fadeDuration = 1f, bool keepActive = false, float inDelay = 0f, float outDelay = 0f)
    {
        Reset();
        isFading = true;
        finished = false;
        startVal = 1.0f;
        targetVal = 0.0f;
        duration = outDuration = fadeDuration;

        if (fader != null && spriteRenderer != null)
        {
            spriteRenderer.alpha = startVal;
            fader.SetActive(true);
            fadeOutCoroutine = StartCoroutine(FadeInOrOut(keepActive, inDelay, outDelay));
        }
    }

    // Fade the black background in or out
    IEnumerator FadeInOrOut(bool keepActive = false, float inDelay = 0f, float outDelay = 0f)
    {
        if(inDelay == 0f)
            yield return new WaitForUnscaledSeconds(waitDelay);
        else
            yield return new WaitForUnscaledSeconds(inDelay);

        float elaspedTime = 0f;
        while (elaspedTime < duration)
        {
            spriteRenderer.alpha = Mathf.Lerp(startVal, targetVal, elaspedTime / duration);
            elaspedTime += Time.unscaledDeltaTime;
            yield return null;
        }

        spriteRenderer.alpha = targetVal;

        if (outDelay == 0f)
            yield return new WaitForUnscaledSeconds(waitDelay);
        else
            yield return new WaitForUnscaledSeconds(outDelay);
        finished = true;
        isFading = false;
        if(!keepActive)
            fader.SetActive(false);

        yield return null;
    }

    IEnumerator FadeInAndOut()
    {
        yield return new WaitForUnscaledSeconds(waitDelay);

        float elaspedTime = 0f;
        while (elaspedTime < duration)
        {
            spriteRenderer.alpha = Mathf.Lerp(startVal, targetVal, elaspedTime / duration);
            elaspedTime += Time.unscaledDeltaTime;
            yield return null;
        }

        spriteRenderer.alpha = targetVal;
        startVal = 1.0f;
        targetVal = 0.0f;

        yield return new WaitForUnscaledSeconds(stayDuration);

        elaspedTime = 0f;
        while (elaspedTime < duration)
        {
            spriteRenderer.alpha = Mathf.Lerp(startVal, targetVal, elaspedTime / duration);
            elaspedTime += Time.unscaledDeltaTime;
            yield return null;
        }

        spriteRenderer.alpha = targetVal;

        yield return new WaitForUnscaledSeconds(waitDelay);

        finished = true;
        isFading = false;
        fader.SetActive(false);
    }

    public void Reset()
    {
        StopCoroutine(fadeInCoroutine);
        StopCoroutine(fadeOutCoroutine);
        StopCoroutine(fadeInAndOutCoroutine);
    }

    public bool IsFading()
    {
        return isFading;
    }

}
