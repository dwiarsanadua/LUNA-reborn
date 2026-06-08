--[[
  Quest 547: [Repeat][Quest] Super Bark
  Level Required: 150
  NPC Start: 99, NPC Complete: 99
  Prerequisites: [546]
  Rewards: EXP=5000000
]]

local fsm = require('fsm_engine')

local quest_547 = fsm:new({
    id = 547,
    name = "[Repeat][Quest] Super Bark",
    level_required = 150,
    npc_start = 99,
    npc_complete = 99,
    prerequisites = {546},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 547: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 547: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 547: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 272,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 547: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 277,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 547: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 547: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 547: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 547: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 547: Rewards given")
                self:give_rewards({
                    exp = 5000000,
                    items = {
                        { item_id = 343, count = 1 },
                        { item_id = 343, count = 1 },
                        { item_id = 343, count = 1 },
                        { item_id = 30000018, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_547