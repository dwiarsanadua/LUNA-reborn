--[[
  Quest 2048: [Daily] Giant Garden 1
  Level Required: 98
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=4469071
]]

local fsm = require('fsm_engine')

local quest_2048 = fsm:new({
    id = 2048,
    name = "[Daily] Giant Garden 1",
    level_required = 98,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2048: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2048: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2048: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 321,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2048: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2048: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2048: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2048: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2048: Rewards given")
                self:give_rewards({
                    exp = 4469071,
                    items = {
                        { item_id = 30000836, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2048