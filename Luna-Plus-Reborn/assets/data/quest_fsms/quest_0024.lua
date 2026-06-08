--[[
  Quest 24: Exterminating Zombies
  Level Required: 22
  NPC Start: 25, NPC Complete: 25
  Rewards: EXP=7721, Gold=2460
]]

local fsm = require('fsm_engine')

local quest_24 = fsm:new({
    id = 24,
    name = "Exterminating Zombies",
    level_required = 22,
    npc_start = 25,
    npc_complete = 25,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 24: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 24: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 24: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 9,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 24: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 45,
                    count = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 24: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 24: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 24: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 24: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 24: Rewards given")
                self:give_rewards({
                    exp = 7721,
                    gold = 2460,
                    items = {
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_24