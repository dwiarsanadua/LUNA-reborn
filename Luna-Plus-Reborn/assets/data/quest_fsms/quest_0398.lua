--[[
  Quest 398: [Quest] Wild eyes
  Level Required: 59
  NPC Start: 93, NPC Complete: 93
  Rewards: EXP=495508, Gold=100000
]]

local fsm = require('fsm_engine')

local quest_398 = fsm:new({
    id = 398,
    name = "[Quest] Wild eyes",
    level_required = 59,
    npc_start = 93,
    npc_complete = 93,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 398: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 398: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 398: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 257,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 398: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 398: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 398: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 398: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 398: Rewards given")
                self:give_rewards({
                    exp = 495508,
                    gold = 100000,
                    items = {
                        { item_id = 300, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_398