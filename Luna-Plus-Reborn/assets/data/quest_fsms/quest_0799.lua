--[[
  Quest 799: Board Management
  Level Required: 66
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=849291, Gold=21542
]]

local fsm = require('fsm_engine')

local quest_799 = fsm:new({
    id = 799,
    name = "Board Management",
    level_required = 66,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 799: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 799: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 799: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 109,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 799: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 799: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 799: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 799: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 799: Rewards given")
                self:give_rewards({
                    exp = 849291,
                    gold = 21542,
                    items = {
                        { item_id = 21000013, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_799