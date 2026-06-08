--[[
  Quest 2050: [Daily] Giant Garden 3
  Level Required: 100
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=5681913
]]

local fsm = require('fsm_engine')

local quest_2050 = fsm:new({
    id = 2050,
    name = "[Daily] Giant Garden 3",
    level_required = 100,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2050: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2050: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2050: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 330,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2050: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 331,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2050: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2050: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2050: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2050: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2050: Rewards given")
                self:give_rewards({
                    exp = 5681913,
                    items = {
                        { item_id = 30000836, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2050