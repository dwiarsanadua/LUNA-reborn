--[[
  Quest 801: [Party] Nasty Things
  Level Required: 67
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=920321, Gold=21542
]]

local fsm = require('fsm_engine')

local quest_801 = fsm:new({
    id = 801,
    name = "[Party] Nasty Things",
    level_required = 67,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 801: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 801: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 801: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 88,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 801: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 801: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 801: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 801: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 801: Rewards given")
                self:give_rewards({
                    exp = 920321,
                    gold = 21542,
                    items = {
                        { item_id = 21001505, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_801