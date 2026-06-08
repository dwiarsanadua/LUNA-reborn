--[[
  Quest 2027: [Daily] Captain Lizardmen
  Level Required: 77
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=856004
]]

local fsm = require('fsm_engine')

local quest_2027 = fsm:new({
    id = 2027,
    name = "[Daily] Captain Lizardmen",
    level_required = 77,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2027: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2027: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2027: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 70,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2027: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2027: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2027: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2027: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2027: Rewards given")
                self:give_rewards({
                    exp = 856004,
                    items = {
                        { item_id = 30000835, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2027