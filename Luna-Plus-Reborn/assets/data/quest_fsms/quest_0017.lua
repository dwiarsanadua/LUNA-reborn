--[[
  Quest 17: Goblin Wrangling
  Level Required: 17
  NPC Start: 21, NPC Complete: 21
  Rewards: EXP=4994, Gold=2320
]]

local fsm = require('fsm_engine')

local quest_17 = fsm:new({
    id = 17,
    name = "Goblin Wrangling",
    level_required = 17,
    npc_start = 21,
    npc_complete = 21,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 17: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 17: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 17: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 6,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 17: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 17: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 49,
                    count = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 17: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 17: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 17: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 17: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 17: Rewards given")
                self:give_rewards({
                    exp = 4994,
                    gold = 2320,
                    items = {
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_17