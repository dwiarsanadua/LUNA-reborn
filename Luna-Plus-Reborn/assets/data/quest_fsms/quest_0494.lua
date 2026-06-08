--[[
  Quest 494: [Special][Quest] Rusty Treent
  Level Required: 61
  NPC Start: 93, NPC Complete: 93
  Rewards: EXP=1040159, Gold=200260
]]

local fsm = require('fsm_engine')

local quest_494 = fsm:new({
    id = 494,
    name = "[Special][Quest] Rusty Treent",
    level_required = 61,
    npc_start = 93,
    npc_complete = 93,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 494: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 494: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 494: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 396,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 494: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 403,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 494: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 494: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 494: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 494: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 494: Rewards given")
                self:give_rewards({
                    exp = 1040159,
                    gold = 200260,
                })
            end,
            transitions = {},
        },
    },
})

return quest_494