--[[
  Quest 909: I want to fly to the sky
  Level Required: 81
  NPC Start: 532, NPC Complete: 532
  Rewards: EXP=2743652
]]

local fsm = require('fsm_engine')

local quest_909 = fsm:new({
    id = 909,
    name = "I want to fly to the sky",
    level_required = 81,
    npc_start = 532,
    npc_complete = 532,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 909: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 532,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 909: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 909: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 271,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 909: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 532,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 909: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 909: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 532,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 909: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 909: Rewards given")
                self:give_rewards({
                    exp = 2743652,
                    items = {
                        { item_id = 395, count = 20 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_909