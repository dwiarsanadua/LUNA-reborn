--[[
  Quest 2032: [Daily] Chopping down Trents
  Level Required: 82
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=1242219
]]

local fsm = require('fsm_engine')

local quest_2032 = fsm:new({
    id = 2032,
    name = "[Daily] Chopping down Trents",
    level_required = 82,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2032: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2032: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2032: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 272,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2032: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2032: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2032: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2032: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2032: Rewards given")
                self:give_rewards({
                    exp = 1242219,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2032