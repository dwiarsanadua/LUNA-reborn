--[[
  Quest 2040: [Daily] A Crack in Space 3
  Level Required: 90
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=2611197
]]

local fsm = require('fsm_engine')

local quest_2040 = fsm:new({
    id = 2040,
    name = "[Daily] A Crack in Space 3",
    level_required = 90,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2040: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2040: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2040: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 288,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2040: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2040: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2040: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2040: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2040: Rewards given")
                self:give_rewards({
                    exp = 2611197,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2040