--[[
  Quest 439: [Party][Quest]The hill of tears
  Level Required: 54
  NPC Start: 118, NPC Complete: 118
  Prerequisites: [438]
  Rewards: EXP=315085
]]

local fsm = require('fsm_engine')

local quest_439 = fsm:new({
    id = 439,
    name = "[Party][Quest]The hill of tears",
    level_required = 54,
    npc_start = 118,
    npc_complete = 118,
    prerequisites = {438},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 439: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 439: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 439: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 97,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 439: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 98,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 439: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 439: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 439: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 439: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 439: Rewards given")
                self:give_rewards({
                    exp = 315085,
                })
            end,
            transitions = {},
        },
    },
})

return quest_439