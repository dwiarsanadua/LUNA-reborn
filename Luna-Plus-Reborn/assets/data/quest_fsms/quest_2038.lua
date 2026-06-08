--[[
  Quest 2038: [Daily] A Crack in Space
  Level Required: 88
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=2053910
]]

local fsm = require('fsm_engine')

local quest_2038 = fsm:new({
    id = 2038,
    name = "[Daily] A Crack in Space",
    level_required = 88,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2038: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2038: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2038: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 283,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2038: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2038: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2038: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2038: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2038: Rewards given")
                self:give_rewards({
                    exp = 2053910,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2038