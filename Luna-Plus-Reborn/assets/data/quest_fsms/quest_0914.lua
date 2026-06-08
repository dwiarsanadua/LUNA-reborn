--[[
  Quest 914: Answer to the Problem?
  Level Required: 82
  NPC Start: 532, NPC Complete: 532
  Rewards: EXP=2484438
]]

local fsm = require('fsm_engine')

local quest_914 = fsm:new({
    id = 914,
    name = "Answer to the Problem?",
    level_required = 82,
    npc_start = 532,
    npc_complete = 532,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 914: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 532,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 914: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 914: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 824,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 914: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 532,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 914: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 914: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 532,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 914: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 914: Rewards given")
                self:give_rewards({
                    exp = 2484438,
                    items = {
                        { item_id = 21000271, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_914