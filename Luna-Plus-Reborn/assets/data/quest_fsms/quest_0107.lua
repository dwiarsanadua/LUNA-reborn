--[[
  Quest 107: Black Bear's Roar
  Level Required: 20
  NPC Start: 56, NPC Complete: 56
  Rewards: EXP=6222, Gold=1390
]]

local fsm = require('fsm_engine')

local quest_107 = fsm:new({
    id = 107,
    name = "Black Bear's Roar",
    level_required = 20,
    npc_start = 56,
    npc_complete = 56,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 107: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 56,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 107: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 107: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 4,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 107: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 56,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 107: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 107: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 56,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 107: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 107: Rewards given")
                self:give_rewards({
                    exp = 6222,
                    gold = 1390,
                    items = {
                        { item_id = 21000008, count = 5 },
                        { item_id = 21000514, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_107