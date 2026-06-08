--[[
  Quest 37: Orc Sorcery
  Level Required: 40
  NPC Start: 34, NPC Complete: 34
  Rewards: EXP=100313, Gold=5550
]]

local fsm = require('fsm_engine')

local quest_37 = fsm:new({
    id = 37,
    name = "Orc Sorcery",
    level_required = 40,
    npc_start = 34,
    npc_complete = 34,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 37: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 37: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 37: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 44,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 37: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 37: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 37: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 37: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 37: Rewards given")
                self:give_rewards({
                    exp = 100313,
                    gold = 5550,
                    items = {
                        { item_id = 15, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_37